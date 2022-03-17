/**
 * @file Message.cpp
 * @brief Implements the Chat::Message class
 * @author Noak Palander
 * @version 1.0
 * @see Message.hpp
 */

#include "Message.hpp"


namespace Chat {
    Message::Message(MessageType type, std::chrono::system_clock::time_point timestamp, std::string data)
        :   type_{type}, timestamp_{timestamp}, data_{std::move(data)} {

        if (!data_.ends_with('\n'))
            data_ += '\n';

        hash_ = std::hash<std::string>()(data_ + std::to_string(timestamp_.time_since_epoch().count()));
    }

    Message::Message(MessageType type, std::chrono::system_clock::time_point timestamp, HashType hash)
        :   type_{type}, timestamp_{timestamp}, data_("\n"), hash_{hash} {}


    /**
     * @brief Constructs a new message formatted to be acknowledged
     * @return the new message
     */
    [[nodiscard]] Message Message::Acknowledge() const {
        // Returns a new acknowledge-message, provides a new timestamp but keeps the hash for validation
        return Message(MessageType::Acknowledge, std::chrono::system_clock::now(), hash_);
    }

    /**
     * @brief Serializes the message into a packet
     * @return the packet corresponding to the current message
     *
     * The packet structure is:
     * 1B = type byte (New/Acknowledge),
     * 8B = hash (consisting of the timestamp and message contents),
     * 8B = timestamp,
     * Remainder = contents, char[] delimited by a newline, this can be empty
     */
    [[nodiscard]]
    std::vector<std::byte> Message::Serialize() const {
        std::vector<std::byte> packet(1 + sizeof(HashType) + sizeof(std::uint64_t) + data_.size());
        std::byte* ptr = packet.data();

        // Type byte
        packet[0] = static_cast<std::byte>(type_);
        ++ptr;

        // Hash
        std::memcpy(ptr, &hash_, sizeof(HashType));
        ptr += sizeof(HashType);

        // Timestamp
        std::uint64_t time = timestamp_.time_since_epoch().count();
        std::memcpy(ptr, &time, sizeof(std::uint64_t));
        ptr += sizeof(std::uint64_t);

        // Content
        std::memcpy(ptr, data_.data(), data_.size());
        return packet;
    }

    /**
     * @brief Deserializes a packet assumed to be structured like the serialization specifies.
     * @param packet deserialized into a message
     * @return the message corresponding to the packet
     */
    [[nodiscard]]
    Message Message::Deserialize(std::vector<std::byte> const& packet) {
        // The beginning of the packet
        std::byte const* data = packet.data();

        // Deserializes the type
        auto const type = static_cast<MessageType>(packet[0]);
        ++data;

        // Deserializes the hash
        HashType hash;
        std::memcpy(&hash, data, sizeof(HashType));
        data += sizeof(HashType);

        // Deserializes the timestamp
        std::uint64_t time{};
        std::memcpy(&time, data, sizeof(std::uint64_t));
        data += sizeof(std::uint64_t);
        using std::chrono::system_clock;
        system_clock::time_point timestamp(duration_cast<system_clock::time_point::duration>(std::chrono::nanoseconds(time)));

        // If the message type is a new message
        if (type == MessageType::New) {
            // Deserializes the message content
            char const *begin = reinterpret_cast<char const *>(data);
            char const *end = reinterpret_cast<char const *>(std::to_address(packet.end()));
            std::string contents(begin, end);

            return Message(type, timestamp, contents);
        }
        // If the message is an acknowledgement
        else {
            // Brings no new content, provide the timestamp and the hash for the acknowledged message
            return Message(type, timestamp, hash);
        }
    }

    /**
     * @brief Constructs a new message (MessageType = New), based on the current time, and contents
     * @param str the message contents
     * @return a new message
     */
    [[nodiscard]]
    Message Message::From(std::string const& str) {
        // Constructs a new message given the current time, and the provided message content
        return Message(MessageType::New, std::chrono::system_clock::now(), str);
    }
}