// Copyright 2024 Mina

#include "webli/router.hpp"
#include <arpa/inet.h>
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <openssl/err.h>
#include <sstream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include <webli/exceptions.hpp>
#include <webli/http.hpp>
#include <webli/server.hpp>

namespace W {
Server::Server(const Router &router)
    : sd(socket(AF_INET, SOCK_STREAM, 0)), router(router),
      ctx(SSL_CTX_new(TLS_server_method())) {
  if ((this->sd == -1) || (!(this->ctx))) {
    perror("[Webli] Server");
    std::exit(EXIT_FAILURE);
    __builtin_unreachable();
  }
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
      ::listen(this->sd, 15) != 0) {
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

    auto t = std::jthread(Server::handle_con, client_sd, this);
    t.detach();
  }
}

void Server::handle_con(int client_sd, Server *server) {
  auto buffer = std::make_unique<std::array<std::uint8_t, 2024>>();
  std::stringstream stream{};

  try {
    auto con = std::make_shared<Con>(client_sd, server->ctx);

    con->read(buffer->data(), 2048);
    stream.write(reinterpret_cast<char *>(buffer->data()), buffer->size());
    buffer.reset();

    Http::Request req_buffer{stream};

    auto resp_buffer = std::make_shared<Http::Response>();
    resp_buffer->setStatusCode(Http::StatusCode::Ok);

    try {
      const auto &handler_vec = server->router.getHandler(
          req_buffer.getMethod(), req_buffer.getPath());
      for (const auto &handler : handler_vec) {
        handler(req_buffer, resp_buffer);
      }
    } catch (WebException::HttpException &e) {
      *resp_buffer = e.getResponse();
    }

    auto resp_str = resp_buffer->build();
    con->write((void *)resp_str.c_str(), resp_str.size());

    std::lock_guard guard(server->print_lock);
    std::cerr << req_buffer.getMethod() << "\t"
              << static_cast<int>(resp_buffer->getStatusCode()) << " | "
              << req_buffer.getPath() << "\n";

  } catch (Exception &e) {
    std::cerr << e.getMessage() << "\n";
  }
}
} // namespace W
