// Copyright 2024 Mina

#include <webli/router.hpp>

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <openssl/err.h>
#include <signal.h>
#include <sstream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include <webli/exceptions.hpp>
#include <webli/http.hpp>
#include <webli/server.hpp>

namespace W {
void sigpipeHandler(int) {
  // SIGPIPE gonna hate
  return;
}

Server::Server(const Router &router, std::size_t buffer_size)
    : sd(socket(AF_INET, SOCK_STREAM, 0)), buffer_size(buffer_size),
      router(router), ctx(SSL_CTX_new(TLS_server_method())) {
  if ((this->sd == -1) || (!(this->ctx))) {
    perror("[Webli] Server");
    std::exit(EXIT_FAILURE);
    __builtin_unreachable();
  }

  // thx to gam3b0y
  signal(SIGPIPE, &sigpipeHandler);
}

Server::~Server() {
  close(this->sd);
  SSL_CTX_free(this->ctx);
}

void Server::ssl_config(std::string_view key_path, std::string_view cert_path) {
  if (SSL_CTX_use_certificate_file(this->ctx, cert_path.data(),
                                   SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    std::exit(EXIT_FAILURE);
    __builtin_unreachable();
  }

  if (SSL_CTX_use_PrivateKey_file(this->ctx, key_path.data(),
                                  SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    std::exit(EXIT_FAILURE);
    __builtin_unreachable();
  }
}

void Server::listen(std::string_view interface, std::uint16_t port) {
  int client_sd;
  unsigned int client_addr_len;
  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(interface.data());

  if (bind(this->sd, reinterpret_cast<struct sockaddr *>(&addr),
           sizeof(addr)) != 0 ||
      ::listen(this->sd, 35) != 0) {
    perror("[Webli] Server::listen");
    std::exit(EXIT_FAILURE);
    __builtin_unreachable();
  }

  std::memset(&addr, 0, sizeof(addr));
  client_addr_len = 0;

  // we reuse the memory place of our sockaddr structure later
  while (running) {
    client_sd = ::accept(this->sd, reinterpret_cast<struct sockaddr *>(&addr),
                         &client_addr_len);
    if (client_sd == -1) {
      perror("[Webli] Accept");
      continue;
    }

    // make explicit copy of addr to new thread
    auto t = std::jthread(Server::handle_con, client_sd, addr.sin_addr, this);
    t.detach();
  }
}

void Server::handle_con(int client_sd, struct in_addr address, Server *server) {
  auto buffer = std::vector<std::uint8_t>();
  buffer.resize(2048);
  std::stringstream stream{};

  try {
    auto con = Con(client_sd, address, server->ctx);

    con.read(buffer.data(), static_cast<int>(buffer.size()));
    stream.write(reinterpret_cast<char *>(buffer.data()), buffer.size());
    buffer.clear();

    Http::Request req_buffer{stream};

    auto resp_buffer = std::make_shared<Http::Response>();
    resp_buffer->setStatusCode(Http::StatusCode::Ok);

    try {
      const auto &handler_vec = server->router.getHandler(
          req_buffer.getMethod(), req_buffer.getPath());
      for (const auto &handler : handler_vec) {
        handler(con, req_buffer, resp_buffer);
      }
    } catch (WebException::HttpException &e) {
      *resp_buffer = e.getResponse();
    }

    auto resp_str = resp_buffer->build();
    con.write(reinterpret_cast<const std::uint8_t *>(resp_str.c_str()),
              static_cast<int>(resp_str.size()));

    std::lock_guard guard(server->print_lock);
    std::cerr << req_buffer.getMethod() << "\t"
              << static_cast<int>(resp_buffer->getStatusCode()) << " | "
              << req_buffer.getPath() << "\n";

  } catch (Exception &e) {
    std::cerr << e.getMessage() << "\n";
  } catch (const std::exception &e) {
    std::cerr << "std::exception: " << e.what() << "\n";
  }
}
} // namespace W
