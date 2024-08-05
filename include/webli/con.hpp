// Copyright 2024 Mina

#pragma once

#include <cstddef>
#include <openssl/ssl.h>

namespace W {
class Con {
public:
  Con(int sd, SSL_CTX *ctx);
  ~Con();

  std::size_t write(void *data, std::size_t data_size) const;

  std::size_t read(void *buffer, std::size_t buffer_size) const;

private:
  void close() noexcept;

  int sd;
  SSL *ssl;
};
} // namespace W
