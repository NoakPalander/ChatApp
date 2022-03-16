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


    [[nodiscard]] Message Message::Acknowledge() const {
        return Message(MessageType::Acknowledge, std::chrono::system_clock::now(), hash_);
    }

    // Serializes the packet data
    // 1B = type byte (New/Acknowledge)     std::byte
    // 8B = hash                            std::uint64_t
    // 8B = timestamp                       std::uint64_t
    // Rem. = contents                      char[] (delimited by \n)
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

    // Deserializes a packet into a message object
    [[nodiscard]]
    Message Message::Deserialize(std::vector<std::byte> const& packet) {
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

        if (type == MessageType::New) {
            // Deserializes the content
            char const *begin = reinterpret_cast<char const *>(data);
            char const *end = reinterpret_cast<char const *>(std::to_address(packet.end()));
            std::string contents(begin, end);

            return Message(type, timestamp, contents);
        }
        else {
            // Acknowledgements bring no new data, and provide the hash for the acknowledged message
            return Message(type, timestamp, hash);
        }
    }

    [[nodiscard]]
    Message Message::From(std::string const& str) {
        return Message(MessageType::New, std::chrono::system_clock::now(), str);
    }
}