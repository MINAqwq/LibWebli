// Copyright 2024 Mina

#include <webli/con.hpp>
#include <webli/exceptions.hpp>

#include <openssl/err.h>
#include <unistd.h>

namespace W {
Con::Con(int sd, SSL_CTX *ctx) : sd(sd), ssl(SSL_new(ctx)) {

  SSL_set_fd(this->ssl, this->sd);

  if (SSL_accept(this->ssl) != 1) {
    ERR_print_errors_fp(stderr);
    this->close();
    throw Exception("TLS Handshake failed");
  }
}

Con::~Con() {
  SSL_shutdown(this->ssl);
  this->close();
}

std::size_t Con::write(void *data, std::size_t data_size) const {
  int ret;

  ret = SSL_write(this->ssl, data, data_size);
  if (ret <= 0) {
    ERR_print_errors_fp(stderr);
    throw Exception("Write to client failed");
  }

  return ret;
}

std::size_t Con::read(void *buffer, std::size_t buffer_size) const {
  int ret;

  ret = SSL_read(this->ssl, buffer, buffer_size);
  if (ret <= 0) {
    ERR_print_errors_fp(stderr);
    throw Exception("Read from client failed");
  }

  return ret;
}

void Con::close() noexcept {
  SSL_free(this->ssl);
  ::close(this->sd);
}
} // namespace W
