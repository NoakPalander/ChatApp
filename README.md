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
Build the executable
```shell
$ cmake --build . --target all
```
or manually invoke the generator like `make`, `ninja`, etc..
###  Documentation
To generate the documentation instead of the command above, run
```shell
$ cmake --build . --target docs
```
the documentation's main page will be found within `docs/html/index.html`, inside the build directory. This requires doxygen to be 
installed.

## Demo
https://user-images.githubusercontent.com/38737983/158841381-98849a2b-4747-423b-bac6-14f79db9705c.mp4
