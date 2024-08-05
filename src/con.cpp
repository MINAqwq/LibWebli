// Copyright 2024 Mina

#include <webli/con.hpp>

#include <openssl/err.h>
#include <unistd.h>

namespace W {
Con::Con(int sd, SSL_CTX *ctx) : sd(sd), ssl(SSL_new(ctx)) {
  SSL_set_fd(this->ssl, this->sd);
  if (SSL_accept(this->ssl) != 1) {
    ERR_print_errors_fp(stderr);
    // TODO: throw exception
    std::exit(2);
  }
}

Con::~Con() {
  SSL_shutdown(this->ssl);
  SSL_free(this->ssl);
  close(this->sd);
}

std::size_t Con::write(void *data, std::size_t data_size) {
  return SSL_write(this->ssl, data, data_size);
}

std::size_t Con::read(void *buffer, std::size_t buffer_size) {
  return SSL_read(this->ssl, buffer, buffer_size);
}
} // namespace W
