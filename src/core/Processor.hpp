/**
 * @file Processor.hpp
 * @brief Provides the declaration to the Chat::Processor class
 * @author Noak Palander
 * @version 1.0
 */

#ifndef CHATAPP_PROCESSOR_HPP
#define CHATAPP_PROCESSOR_HPP

#include "asio/io_service.hpp"
#include "asio/executor_work_guard.hpp"
#include "asio/ip/tcp.hpp"
#include "Message.hpp"
#include "../core/Mode.hpp"
#include <memory>
#include <functional>
#include <thread>


namespace Chat {
    /**
     * @class Chat::Processor
     * @brief Contains the server or client, depending on what mode was passed
     * @author Noak Palander
     */
    class Processor {
    public:
        /**
         * @brief Constructs a server
         * @param port the port to be used
         * @param onReceive a callback that is invoked when a message is received
         * @param onConnected a callback that is invoked when a client connects
         * @param onConnectionLost a callback that is invoked when a client disonnects
         */
        Processor(int port,
                  std::function<void(Chat::Message const&)> onReceive,
                  std::function<void()> onConnected,
                  std::function<void()> onConnectionLost);

        /**
         * @brief Constructs a client
         * @param port the port to be used
         * @param address the address to be used
         * @param onReceive a callback that is invoked when a message is received
         * @param onConnected a callback that is invoked when the connection to the server is established
         * @param onConnectionLost a callback that is invoked if the connection to the server is lost
         */
        Processor(int port, std::string const& address,
                  std::function<void(Chat::Message const&)> onReceive,
                  std::function<void()> onConnected,
                  std::function<void()> onConnectionLost);

        ~Processor();

        /**
         * @brief Broadcasts a message to the recipient, can be used in both configurations
         * @param message the message that should be sent the server/client
         */
        void Transmit(Chat::Message const& message);

    private:
        /**
         * @brief Internal, starts to accept clients, can only be used as a server
         */
        void Accept();

        /**
         * @brief Internal, is invoked when a client connects
         * @param ec an error code provided by asio::async_connect
         */
        void HandleAccept(asio::error_code ec);

        /**
         * @brief Internal, starts to receive incoming data, can be used in both configurations
         */
        void Receive();

        /**
         * @brief Internal, is invoked when a message was received
         * @param ec an error code provided by asio::async_read_until
         * @param bytes the number of bytes received
         */
        void Reader(asio::error_code ec, std::size_t bytes);

        Mode mode_;                                                           /**< the current configuration */
        std::vector<std::byte> buffer_;                                       /**< The packet buffer for receiving data */
        std::jthread runner_;                                                 /**< Starts the io_service on a background thread */

        asio::io_service service_;                                            /**< the io service that handles async events */
        std::unique_ptr<asio::ip::tcp::socket> socket_;                       /**< pointer to a tcp socket */
        asio::executor_work_guard<asio::io_context::executor_type> guard_;    /**< keeps the io_service running */
        std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;                   /**< pointer to an acceptor for the server */

        // Event callbacks for the UI
        std::function<void(Chat::Message const&)> onReceive_;
        std::function<void()> onConnect_;
        std::function<void()> onDisconnect_;
    };
}

#endif