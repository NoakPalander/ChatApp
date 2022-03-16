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
    class Processor {
    public:
        Processor(int port,
                  std::function<void(Chat::Message const&)> onReceive,
                  std::function<void()> onConnected,
                  std::function<void()> onConnectionLost);

        Processor(int port, std::string const& address,
                  std::function<void(Chat::Message const&)> onReceive,
                  std::function<void()> onConnected,
                  std::function<void()> onConnectionLost);

        ~Processor();

        void Transmit(Chat::Message const& message);

    private:
        void Accept();
        void HandleAccept(asio::error_code ec);

        void Receive();
        void Reader([[maybe_unused]] asio::error_code ec, std::size_t bytes);

        Mode mode_;
        std::vector<std::byte> buffer_;
        std::jthread runner_;

        asio::io_service service_;
        std::unique_ptr<asio::ip::tcp::socket> socket_;
        asio::executor_work_guard<asio::io_context::executor_type> guard_;
        std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;

        // Callbacks for the UI
        std::function<void(Chat::Message const&)> onReceive_;
        std::function<void()> onConnect_;
        std::function<void()> onDisconnect_;
    };
}

#endif