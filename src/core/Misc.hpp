#ifndef MISC_HPP
#define MISC_HPP

#include <QString>
#include <string_view>
#include <algorithm>
#include "fmt/format.h"
#include "fmt/color.h"
#include <iostream>

namespace Misc {
    template<typename... Args>
    [[nodiscard]] inline QString QFormat(std::string_view format, Args&&... args) {
        QString out;
        fmt::vformat_to(std::back_inserter(out), format, fmt::make_format_args(std::forward<Args>(args)...));
        return out;
    }

    template<typename... Args>
    inline constexpr void Debug(std::string_view format, Args&&... args) {
    #ifdef DEBUG
        fmt::print(::stderr, fmt::emphasis::underline | fg(fmt::color::red), format, std::forward<Args>(args)...);
        std::cerr.flush();
    #endif
    }
}


#endif //MISC_HPP
