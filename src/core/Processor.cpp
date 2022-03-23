/**
 * @file Processor.cpp
 * @brief Implements the Chat::Processor class
 * @author Noak Palander
 * @version 1.0
 * @see Processor.hpp
 */

#include "Processor.hpp"

#include "asio/read_until.hpp"
#include "asio/write.hpp"
#include "Misc.hpp"
#include "Message.hpp"
#include <utility>

namespace Chat {
    /**
     * @brief Constructs a server
     * @param port the port to be used
     * @param onReceive a callback that is invoked when a message is received
     * @param onConnected a callback that is invoked when a client connects
     * @param onConnectionLost a callback that is invoked when a client disonnects
     */
    Processor::Processor(int port,
                         std::function<void(Chat::Message const&)> onReceive,
                         std::function<void()> onConnect,
                         std::function<void()> onDisconnect)
        :   mode_{Mode::Server},
            socket_{std::make_unique<asio::ip::tcp::socket>(service_)},
            acceptor_{std::make_unique<asio::ip::tcp::acceptor>(service_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))},
            onReceive_{std::move(onReceive)},
            onConnect_{std::move(onConnect)},
            onDisconnect_{std::move(onDisconnect)}
    {
        Misc::Debug("Constructed a server!\n");
        acceptor_->set_option(asio::ip::tcp::acceptor::reuse_address(true));

        // Starts accepting clients
        Accept();

        // Start listening for messages
        Receive();
        runner_ = std::jthread([this](std::stop_token token){
            while(!token.stop_requested()) {
                service_.run();
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                service_.restart();
            }
        });
    }

    Processor::Processor(int port, std::string const& address,
                         std::function<void(Chat::Message const&)> onReceive,
                         std::function<void()> onConnect,
                         std::function<void()> onDisconnect)
        :   mode_{Mode::Client},
            socket_{std::make_unique<asio::ip::tcp::socket>(service_)},
            onReceive_{std::move(onReceive)},
            onConnect_{std::move(onConnect)},
            onDisconnect_{std::move(onDisconnect)}
    {
        Misc::Debug("Starting client!\n");

        // Attempt to connect to the server
        socket_->async_connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(address), port), [this](asio::error_code code) {
            // When connection was successful
            if (code.value() == 0)
                onConnect_();
        });

        // Start listening for messages
        Receive();
        runner_ = std::jthread([this](std::stop_token token){
            while(!token.stop_requested()) {
                service_.run();
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                service_.restart();
            }
        });
    }

    Processor::~Processor() {
        runner_.request_stop();
        asio::post(service_, [this]{
            service_.stop();
        });

        runner_.join();

        if (socket_->is_open()) {
            asio::error_code ec;
            socket_->shutdown(asio::ip::tcp::socket::shutdown_both, ec);
            socket_->close();
        }

        Misc::Debug("Stopping {}\n", mode_);
    }

    void Processor::Accept() {
        acceptor_->async_accept(*socket_, std::bind_front(&Processor::HandleAccept, this));
    }

    void Processor::HandleAccept(asio::error_code ec) {
        // A client connected
        if (ec.value() == 0)
            onConnect_();

        Accept();
    }

    void Processor::Receive() {
        // Reads into buffer_ when it receives some data, then invoeks the Reader callback
        asio::async_read_until(*socket_, asio::dynamic_buffer(buffer_), '\n', std::bind_front(&Processor::Reader, this));
    }

    // If an incomming message was received
    void Processor::Reader(asio::error_code ec, std::size_t bytes) {
        // If the processor (server) detected a client disconnect
        if (ec == asio::error::eof || ec == asio::error::interrupted) [[unlikely]] {
            if (mode_ == Mode::Server)
                socket_ = std::make_unique<asio::ip::tcp::socket>(service_);

            onDisconnect_();
        }
        // Otherwise, we detected a message or something else from async_read_until
        else [[likely]] {
            // Incoming messages exist
            if (bytes > 0) {
                auto received = Chat::Message::Deserialize(buffer_);
                onReceive_(received);
                buffer_.clear();

                // If the message we received was a new message, send an acknowledgment
                if (received.Type() == Chat::MessageType::New) {
                    auto const acknowledgement = received.Acknowledge();
                    Transmit(acknowledgement);
                }
            }
        }

        Receive();
    }

    // Sends a new message
    void Processor::Transmit(Chat::Message const& message) {
        // Serializes the message and attempt to send it
        auto packet = message.Serialize();
        asio::async_write(*socket_, asio::buffer(packet), []([[maybe_unused]] asio::error_code ec, std::size_t bytes) {
            Misc::Debug("Transmitting {} bytes!\n", bytes);
        });
    }
}