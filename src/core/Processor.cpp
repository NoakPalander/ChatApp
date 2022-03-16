#include "Processor.hpp"

#include <utility>

#include "asio/read_until.hpp"
#include "asio/write.hpp"
#include "Misc.hpp"
#include "Message.hpp"


namespace Chat {
    Processor::Processor(int port,
                         std::function<void(Chat::Message const&)> onReceive,
                         std::function<void()> onConnect,
                         std::function<void()> onDisconnect)
         : mode_{Mode::Server},
           socket_{std::make_unique<asio::ip::tcp::socket>(service_)},
           guard_{asio::make_work_guard(service_)},
           acceptor_{std::make_unique<asio::ip::tcp::acceptor>(service_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))},
           onReceive_{std::move(onReceive)},
           onConnect_{std::move(onConnect)},
           onDisconnect_{std::move(onDisconnect)}
    {
        Misc::Debug("Constructed a server!\n");

        acceptor_->set_option(asio::ip::tcp::acceptor::reuse_address(true));
        Accept();

        Receive();
        runner_ = std::jthread([this]{ service_.run(); });
    }

    Processor::Processor(int port, std::string const& address,
                         std::function<void(Chat::Message const&)> onReceive,
                         std::function<void()> onConnect,
                         std::function<void()> onDisconnect)
        : mode_{Mode::Client},
          socket_{std::make_unique<asio::ip::tcp::socket>(service_)},
          guard_{asio::make_work_guard(service_)},
          onReceive_{std::move(onReceive)},
          onConnect_{std::move(onConnect)},
          onDisconnect_{std::move(onDisconnect)}
    {
        Misc::Debug("Starting client!\n");
        socket_->async_connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(address), port), [this](asio::error_code code) {
            if (code.value() == 0)
                onConnect_();
        });

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
            onConnect_();

        Accept();
    }

    void Processor::Receive() {
        asio::async_read_until(*socket_, asio::dynamic_buffer(buffer_), '\n',
                               std::bind_front(&Processor::Reader, this));
    }

    void Processor::Reader(asio::error_code ec, std::size_t bytes) {
        // If the processor (server) detected a client disconnect
        if (ec == asio::error::eof || ec == asio::error::interrupted) {
            socket_ = std::make_unique<asio::ip::tcp::socket>(service_);
            onDisconnect_();
        }
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

    void Processor::Transmit(Chat::Message const& message) {
        auto packet = message.Serialize();
        asio::async_write(*socket_, asio::buffer(packet),
                          []([[maybe_unused]] asio::error_code ec, std::size_t bytes) {
            Misc::Debug("Transmitting {} bytes!\n", bytes);
        });
    }
}