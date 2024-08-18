# LibWebli

This is the library powering the same named web framework (in work). LibWebli provides subroutines for hosting, connection handling, runtime exceptions and more. Made with the goal to make api programming in C++ less painful. But of course you can use it however you want.


## Dependencies

- Libc
- STL (C++20)
- BSD/Posix Sockets
- OpenSSL


## Features

Here is a list of the current and planned features. 

- [X] JSON (through nlohmann)
- [X] .env
- [ ] HTTP
- - [X] Status Codes
- - [X] Header
- - [X] JSON
- - [X] Requests
- - [X] Responses
- - [ ] HEAD
- - [ ] OPTIONS
- - [X] Cookies
- [ ] Router
- - [X] Static Routes
- - [X] Custom Methods
- - [ ] Dynamic Routes
- [X] Server
- - [X] TLS (through openssl)
- - [X] Multithreading
- - [ ] Sendfile (?)
- [ ] Client
- - [ ] HTTPS
- - [ ] TLS over TCP
- [X] Storage API
- - [X] Load from filesystem
