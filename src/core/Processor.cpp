#include "Processor.hpp"

#include <utility>

#include "asio/read_until.hpp"
#include "asio/write.hpp"
#include "Misc.hpp"
#include "Message.hpp"


namespace Chat {
    Processor::Processor(int port, std::function<void(Chat::Message const&)> onReceive, std::function<void()> onClientLeft)
        :   mode_{Mode::Server},
            socket_{std::make_unique<asio::ip::tcp::socket>(service_)},
            guard_{asio::make_work_guard(service_)},
            acceptor_{std::make_unique<asio::ip::tcp::acceptor>(service_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))},
            onReceive_{std::move(onReceive)},
            onClientLeft_{std::move(onClientLeft)}
    {
        Misc::Debug("Constructed a server!\n");

        acceptor_->set_option(asio::ip::tcp::acceptor::reuse_address(true));
        Accept();

        Receive();
        runner_ = std::jthread([this]{ service_.run(); });
    }

    Processor::Processor(int port, std::string const& address, std::function<void(Chat::Message const&)> onReceive)
        :   mode_{Mode::Client},
            socket_{std::make_unique<asio::ip::tcp::socket>(service_)},
            guard_{asio::make_work_guard(service_)},
            onReceive_(std::move(onReceive))
    {
        Misc::Debug("Starting client!\n");
        socket_->async_connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(address), port),
                              []([[maybe_unused]] asio::error_code code){});

        Receive();
        runner_ = std::jthread([this]{ service_.run(); });
    }

    Processor::~Processor() {
        guard_.reset();
        service_.stop();

        if (socket_->is_open()) {
            socket_->shutdown(asio::ip::tcp::socket::shutdown_both);
            socket_->close();
        }
    }

    void Processor::Accept() {
        acceptor_->async_accept(*socket_, std::bind_front(&Processor::HandleAccept, this));
    }

    void Processor::HandleAccept(asio::error_code ec) {
        if (ec.value() == 0)
            Misc::Debug("A client connected!\n");

        Accept();
    }

    void Processor::Receive() {
        asio::async_read_until(*socket_, asio::dynamic_buffer(buffer_), '\n',
                               std::bind_front(&Processor::Reader, this));
    }

    void Processor::Reader(asio::error_code ec, std::size_t bytes) {
        // If the processor (server) detected a client disconnect
        if ((ec == asio::error::eof || ec == asio::error::interrupted) && mode_ == Mode::Server) [[unlikely]] {
            // Recreate socket for a new connection
            socket_ = std::make_unique<asio::ip::tcp::socket>(service_);
            // Invoke callback
            onClientLeft_();
        }
        else [[likely]] {
            // Incoming messages exist
            if (bytes > 0) {
                auto received = Chat::Message::Deserialize(buffer_);
                onReceive_(received);
                buffer_.clear();

                // If the message we received was a new message, send an acknowledgment
                if (received.Type() == Chat::MessageType::New) {
                    auto const ack = received.Acknowledge();
                    Transmit(ack);
                }
            }
        }

        Receive();
    }

    void Processor::Transmit(Chat::Message const& message) {
        auto packet = message.Serialize();
        asio::async_write(*socket_, asio::buffer(packet),
                          []([[maybe_unused]] asio::error_code ec, std::size_t bytes) {
            Misc::Debug("Transmitting {} bytes!\n", bytes);
        });
    }
}