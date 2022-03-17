/**
 * @file Misc.hpp
 * @brief Contains various helpers for the application
 * @author Noak Palander
 * @version 1.0
 */

#ifndef CHATAPP_MISC_HPP
#define CHATAPP_MISC_HPP

#include <QString>
#include <string_view>
#include <algorithm>
#include "fmt/format.h"
#include "fmt/color.h"
#include <iostream>

namespace Misc {
    /**
     * @brief Provides an fmt::format version that produces a QString instead of an std::string
     * @param format the format string, follows fmtlib formatting
     * @param args the args that should be formatted into the string
     * @return the formatted output as a QString
     */
    template<typename... Args>
    [[nodiscard]] inline QString QFormat(std::string_view format, Args&&... args) {
        QString out;
        fmt::vformat_to(std::back_inserter(out), format, fmt::make_format_args(std::forward<Args>(args)...));
        return out;
    }

    /**
     * @brief Prints some color-coded string to stderr and flushes it, if the current build mode is Debug, otherwise it does nothing
     * @param format the format string, follows fmtlib formatting
     * @param args the args that should be formatted into the string
     */
    template<typename... Args>
    inline constexpr void Debug(std::string_view format, Args&&... args) {
    #ifdef DEBUG
        // Underlined red-color
        fmt::print(::stderr, fmt::emphasis::underline | fg(fmt::color::red), format, std::forward<Args>(args)...);
        std::cerr.flush();
    #endif
    }
}


#endif //CHATAPP_MISC_HPP
