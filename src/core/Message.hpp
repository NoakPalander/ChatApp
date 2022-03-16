#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdint>
#include <string_view>
#include <string>
#include <vector>
#include <cstring>
#include <chrono>

namespace Chat {
    enum class MessageType : unsigned char {
        New = 0,
        Acknowledge = 1
    };

    class Message {
    public:
        using HashType = std::size_t;

        Message(MessageType type, std::chrono::system_clock::time_point timestamp, std::string data);
        Message(MessageType type, std::chrono::system_clock::time_point timestamp, HashType hash);

        ~Message() = default;

        bool operator==(Message const& rhs) const noexcept { return hash_ == rhs.hash_; }

        [[nodiscard]] Message Acknowledge() const;

        // Serializes the packet data
        // 1B = type byte (New/Acknowledge)     std::byte
        // 8B = hash                            (HashType = std::size_t)
        // 8B = timestamp                       std::uint64_t
        // Rem. = contents                      char[] (delimited by \n)
        [[nodiscard]] std::vector<std::byte> Serialize() const;

        // Deserializes a packet into a message
        [[nodiscard]] static Message Deserialize(std::vector<std::byte> const& packet);
        // Constructs a new message from some data
        [[nodiscard]] static Message From(std::string const& str);

        [[nodiscard]] MessageType Type() const noexcept { return type_; }
        [[nodiscard]] std::chrono::system_clock::time_point Timestamp() const noexcept { return timestamp_; }
        [[nodiscard]] std::string Contents() const noexcept { return data_; }
        [[nodiscard]] HashType Identifier() const noexcept { return hash_; }


    private:
        MessageType type_;
        std::chrono::system_clock::time_point timestamp_;
        std::string data_;
        HashType hash_;
    };
}

#endif