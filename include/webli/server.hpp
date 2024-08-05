// Copyright 2024 Mina

#pragma once

#include <webli/con.hpp>

#include <cstdint>
#include <memory>
#include <openssl/ssl.h>
#include <string_view>

namespace W {
class Server {
public:
  Server();
  ~Server();

  void ssl_config(std::string_view key_path, std::string_view cert_path);

  void listen(std::string_view interface, std::uint16_t port);

  static void handle_con(std::shared_ptr<Con> con);

private:
  int sd;
  bool running{true};
  SSL_CTX *ctx;
};
} // namespace W
