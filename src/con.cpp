// Copyright 2024 Mina

#include <webli/con.hpp>
#include <webli/exceptions.hpp>

#include <openssl/err.h>
#include <unistd.h>

namespace W {
Con::Con(int sd, struct in_addr address, SSL_CTX *ctx)
    : sd(sd), address(address), ssl(SSL_new(ctx)) {

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

std::size_t Con::write(const std::uint8_t *data, int data_size) const {
  int ret;

  ret = SSL_write(this->ssl, data, data_size);
  if (ret <= 0) {
    ERR_print_errors_fp(stderr);
    throw Exception("Write to client failed");
  }

  return ret;
}

std::size_t Con::read(std::uint8_t *buffer, int buffer_size) const {
  int ret;

  ret = SSL_read(this->ssl, buffer, buffer_size);
  if (ret <= 0) {
    ERR_print_errors_fp(stderr);
    throw Exception("Read from client failed");
  }

  return ret;
}

struct in_addr Con::getAddress() { return this->address; }

void Con::close() noexcept {
  SSL_free(this->ssl);
  ::close(this->sd);
}
} // namespace W
