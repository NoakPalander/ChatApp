include(FetchContent)

FetchContent_Declare(
    asiocmake
    GIT_REPOSITORY "https://github.com/OlivierLDff/asio.cmake"
    GIT_TAG        "main")

FetchContent_Declare(
    fmt
    GIT_REPOSITORY "https://github.com/fmtlib/fmt"
    GIT_TAG        "master")

FetchContent_MakeAvailable(asiocmake)
FetchContent_MakeAvailable(fmt)
