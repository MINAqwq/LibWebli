// Copyright 2024 Mina

#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <openssl/err.h>
#include <sys/socket.h>
#include <unistd.h>
#include <webli/server.hpp>

namespace W {
Server::Server()
    : sd(socket(AF_INET, SOCK_STREAM, 0)),
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
  addr.sin_addr.s_addr = htonl(inet_addr(interface.data()));

  if (bind(this->sd, reinterpret_cast<struct sockaddr *>(&addr),
           sizeof(addr)) != 0 ||
      ::listen(this->sd, 15) != 0) {
    perror("[Webli] Server::listen");
    std::exit(EXIT_FAILURE);
    __builtin_unreachable();
  }

  // we reuse the memory place of our sockaddr structure later
  while (running) {
    memset(&addr, 0, sizeof(addr));
    client_addr_len = 0;
    client_sd = ::accept(this->sd, reinterpret_cast<struct sockaddr *>(&addr),
                         &client_addr_len);
    if (client_sd == -1) {
      perror("[Webli] Accept");
      continue;
    }

    Server::handle_con(std::make_shared<Con>(client_sd, this->ctx));
  }
};

void Server::handle_con(std::shared_ptr<Con> con) {
  std::cerr << "writing stuff yeah\n";
  con->write((void *)"Deine Mama\n", 11);
}
} // namespace W
