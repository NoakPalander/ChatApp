#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "asio/io_service.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/streambuf.hpp"
#include "asio/read_until.hpp"
#include "asio/write.hpp"
#include "../core/Message.hpp"
#include "../core/Misc.hpp"
#include <thread>
#include <deque>
#include <string>

/*class Chat::ProcessorImpl {
public:
    void ClientLeft() {}

    asio::ip::tcp::socket& GetSocket() {
        return socket_;
    }

    asio::io_service& GetService() {
        return service_;
    }

private:
    asio::ip::tcp::socket socket_;
    asio::io_service service_;
};*/

/*class Client {
private:
    Chat::Base processor_;
    asio::ip::tcp::socket socket_;

public:
    Client(std::string const& address, int port, auto onReceive)
        :   processor_{
                .guard = asio::make_work_guard(processor_.service),
                .onReceive = onReceive
            },
            socket_{processor_.service}
    {
        Misc::Debug("Starting client!\n");
        socket_.async_connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(address), port),
                              []([[maybe_unused]] asio::error_code code){});

        Receive();
        processor_.runner = std::jthread([this]{ processor_.service.run(); });
    }

    ~Client() {
        guard_.reset();
        service_.stop();

        if (socket_.is_open()) {
            socket_.shutdown(asio::ip::tcp::socket::shutdown_both);
            socket_.close();
        }
    }

    asio::ip::tcp::socket& GetSocket() {
        return socket_;
    }

    void Transmit(Chat::Message message) {
        auto packet = message.Serialize();
        asio::async_write(socket_, asio::buffer(packet), []([[maybe_unused]] asio::error_code ec, std::size_t bytes){
            Misc::Debug("Transmitting {} bytes!\n", bytes);
        });
    }

    void Receive() {
        asio::async_read_until(socket_, asio::dynamic_buffer(buffer_), '\n', std::bind_front(&Client::Reader, this));
    }

    void Reader([[maybe_unused]] asio::error_code ec, std::size_t bytes) {
        // Incoming messages exist
        if (bytes > 0) {
            auto received = Chat::Message::Deserialize(buffer_);
            onReceive_(received);
            buffer_.clear();

            // If the message we received was a new message, send an acknowledgment
            if (received.Type() == Chat::MessageType::New) {
                Transmit(received.Acknowledge());
            }
        }

        Receive();
    }
};*/

#endif