# LibWebli

This is the library powering the same named web framework (in work). LibWebli provides subroutines for hosting, connection handling, runtime exceptions and more. Made with the goal to make api programming in C++ less painful. But of course you can use it however you want.

## Dependencies

- Libc
- STL (C++20)
- BSD/Posix Sockets
- OpenSSL

## Add to project

To add webli to your cmake project you can use something like this:

```cmake
# enable webli's https client api
set(WEBLI_CLIENT ON)

add_subdirectory(<webli_src>)

# define your target ...

target_include_directories(target PRIVATE <webli_src>/include <webli_src>/dep/json)

target_link_libraries(target PRIVATE webli)
```

Where `<webli_src>` is the name of the folder containing WebLi's `CMakeLists.txt`.

## Features

Here is a list of the current and planned features.

- [x] JSON (through nlohmann)
- [x] .env
- [ ] HTTP
- - [x] Status Codes
- - [x] Header
- - [x] JSON
- - [x] Requests
- - [x] Responses
- - [ ] HEAD
- - [ ] OPTIONS
- - [x] Cookies
- [x] Websocket
- [ ] Router
- - [x] Static Routes
- - [x] Custom Methods
- - [ ] Dynamic Routes
- [x] Server
- - [x] TLS (through openssl)
- - [x] Multithreading
- [x] Client
- - [x] HTTPS
- [x] Storage API
- - [x] Load from filesystem
