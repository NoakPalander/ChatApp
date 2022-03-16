# ChatApp
Provides a Server-Client TCP connection for chatting, and recalls the response time for message delivery.

## Requirements / Recommendations
* A C++20 compliant, or partially compliant compiler. The development was done using GCC 11.1.
* CMake, the development was done using CMake 3.22.2, but it only targets version 3.8.
* Qt 5.1 for the UI, will have to be preinstalled, can be done via the package manager on Linux.
* Make or another CMake-compatible build system.

## Build steps for Linux
After cloning the project, create a build directory and enter it
```shell
$ mkdir build && cd build 
```
Generate the build files using CMake, this will also download the dependencies [asio](https://github.com/OlivierLDff/asio.cmake)
and [fmtlib](https://github.com/fmtlib/fmt).
```shell
$ cmake -S ../ -B . -DCMAKE_BUILD_TYPE=Release # optionally Debug for development purposes
```
Finally build the executable.
```shell
$ make
```
