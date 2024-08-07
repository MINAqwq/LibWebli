// Copyright 2024 Mina

#pragma once

#include <cstddef>
#include <openssl/ssl.h>

namespace W {
/**
 * @brief TLS TCP Client Connection
 *
 */
class Con {
public:
  /**
   * @brief Construct a new Con object
   *
   * @param sd socket descriptor
   * @param ctx tls context
   */
  Con(int sd, SSL_CTX *ctx);
  ~Con();

  /**
   * @brief write data onto the buffer
   *
   * @param data pointer to data
   * @param data_size size to write in bytes
   * @return std::size_t - bytes written
   */
  std::size_t write(void *data, std::size_t data_size) const;

  /**
   * @brief read data from socket into buffer
   *
   * @param buffer pointer to buffer
   * @param buffer_size size to read in bytes
   * @return std::size_t - bytes read
   */
  std::size_t read(void *buffer, std::size_t buffer_size) const;

private:
  /**
   * @brief free tls context and close socket
   * @note this function won't shutdown the tls session
   *
   */
  void close() noexcept;

  /** @brief socket descriptor  */
  int sd;

  /** @brief tls context  */
  SSL *ssl;
};
} // namespace W
