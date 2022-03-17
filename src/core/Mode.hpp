/**
 * @file Mode.hpp
 * @brief Contains the processor configuration mode, and some helpers to the enum
 * @author Noak Palander
 * @version 1.0
 */

#ifndef CHATAPP_MODE_HPP
#define CHATAPP_MODE_HPP

#include <fmt/format.h>
#include <string_view>

namespace Chat {
    /**
     * @enum Chat::Mode
     * @brief The type of configuration
     * @author Noak Palander.
     */
    enum class Mode {
        Server = 0, /**< The configuration is to be used as a server */
        Client = 1  /**< The configuration is to be used as a client */
    };
}

/**
 * @brief Inverts the passed mode, Mode::Server becomes Mode::Client, and vice versa.
 * @param m the mode to invert
 * @return the inverted mode
 */
inline constexpr Chat::Mode operator!(Chat::Mode m) noexcept {
    return static_cast<Chat::Mode>(!static_cast<int>(m));
}

/**
 * @struct fmt::formatter<Chat::Mode>
 * @brief Enables fmtlib formatting for Chat::Mode, this essentially stringifies the enum values.
 * @author Noak Palander
 *
 * The reason why the `operator<<` isnt overloaded as an alternative, is because that might add a level of obscurity incase bitwise
 * shifting were to be desired in the future.
 */
template<>
struct fmt::formatter<Chat::Mode> : formatter<std::string_view> {
    /**
     * @param mode the chat mode to "stringify"
     * @param ctx the formatting context that is provided by fmt::format
     * @return the formatted mode
     */
    auto format(Chat::Mode mode, fmt::format_context& ctx) {
        return formatter<std::string_view>::format(mode == Chat::Mode::Server ? "Server" : "Client", ctx);
    }
};


#endif // CHATAPP_MODE_HPP