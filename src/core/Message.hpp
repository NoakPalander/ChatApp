/**
 * @file Message.hpp
 * @brief Contains the declarations of the Message class and the MessageType enum
 * @author Noak Palander
 * @version 1.0
 */

#ifndef CHATAPP_MESSAGE_HPP
#define CHATAPP_MESSAGE_HPP

#include <cstdint>
#include <string_view>
#include <string>
#include <vector>
#include <cstring>
#include <chrono>

namespace Chat {
    /**
     * @enum MessageType
     * @brief The type of message
     * @author Noak Palander
     */
    enum class MessageType : unsigned char {
        New = 0,            /**< Indicates that the message is a completely new message */
        Acknowledge = 1     /**< Indiciates that the message is an acknowledgement to a previous one */
    };

    /**
     * @class Chat::Message
     * @brief The class that's used for transmitting messages between the client and server
     * @author Noak Palander
     */
    class Message {
    public:
        using HashType = std::size_t;

        Message(MessageType type, std::chrono::system_clock::time_point timestamp, std::string data);
        Message(MessageType type, std::chrono::system_clock::time_point timestamp, HashType hash);

        ~Message() = default;

        /**
         * @brief Compares a message to another one based on the hash
         * @param rhs the other message object to compare
         * @return the comparison between two messages' hashes
         */
        bool operator==(Message const& rhs) const noexcept { return hash_ == rhs.hash_; }

        /**
         * @brief Constructs a new message formatted to be acknowledged
         * @return the new message
         */
        [[nodiscard]] Message Acknowledge() const;

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
        [[nodiscard]] std::vector<std::byte> Serialize() const;

        /**
         * @brief Deserializes a packet assumed to be structured like the serialization specifies.
         * @param packet deserialized into a message
         * @return the message corresponding to the packet
         */
        [[nodiscard]] static Message Deserialize(std::vector<std::byte> const& packet);

        /**
         * @brief Constructs a new message (MessageType = New), based on the current time, and contents
         * @param str the message contents
         * @return a new message
         */
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

#endif // CHATAPP_MESSAGE_HPP