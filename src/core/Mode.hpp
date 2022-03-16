#ifndef CHATAPP_MODE_HPP
#define CHATAPP_MODE_HPP

#include <fmt/format.h>
#include <string_view>

enum class Mode {
    Server = 0,
    Client = 1
};

// Returns the opposite mode
inline constexpr Mode operator!(Mode m) noexcept {
    return static_cast<Mode>(!static_cast<int>(m));
}

template<>
struct fmt::formatter<Mode> : formatter<std::string_view> {
    auto format(Mode mode, fmt::format_context& ctx) {
        return formatter<std::string_view>::format(mode == Mode::Server ? "Server" : "Client", ctx);
    }
};


#endif // CHATAPP_MODE_HPP