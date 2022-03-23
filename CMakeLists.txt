cmake_minimum_required(VERSION 3.8)

# Overall properties used by cmake or doxygen
set(PROJECT_NAME ChatApp)
set(PROJECT_VERSION 1.0.1)
set(PROJECT_BREIF "Provides a Server-Client TCP connection for chatting, and recalls the response time for message delivery.")

project(${PROJECT_NAME})

# UI compiling
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt is required for the UI
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Includes the cmake-doxygen configuration
include(${CMAKE_SOURCE_DIR}/src/docs/CMakeLists.txt)

# Includes the external dependencies
include(${CMAKE_SOURCE_DIR}/ext/CMakeLists.txt)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/core/Misc.hpp
    src/core/Processor.hpp
    src/core/Processor.cpp
    src/core/Message.hpp
    src/core/Message.cpp
    src/ui/MainWindow.ui
    src/ui/MainWindow.cpp
    src/ui/MainWindow.hpp
    src/ui/widgets/AppWidget.ui
    src/ui/widgets/AppWidget.cpp
    src/ui/widgets/AppWidget.hpp
    src/ui/widgets/ModeSelect.ui
    src/ui/widgets/ModeSelect.cpp
    src/ui/widgets/ModeSelect.hpp)

# C++20
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Debug mode, harder warnings, sanitizers and debug logs
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    message("Configuring debug mode")

    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic-errors -O0 -g -fsanitize=undefined,leak,address)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined,leak,address)

# Release mode, optimizations
elseif(CMAKE_BUILD_TYPE MATCHES "Release")
    message("Configuring release mode")

    target_compile_definitions(${PROJECT_NAME} PRIVATE RELEASE)
    target_compile_options(${PROJECT_NAME} PRIVATE -O3 -Wpedantic)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    pthread
    Qt5::Widgets
    asio::asio
    fmt::fmt)

