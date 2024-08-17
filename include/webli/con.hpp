// Copyright 2024 Mina

#pragma once

#include <arpa/inet.h>
#include <cstddef>
#include <cstdint>

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
   * @param address internet address
   * @param ctx tls context
   */
  Con(int sd, struct in_addr address, SSL_CTX *ctx);
  ~Con();

  /**
   * @brief write data onto the buffer
   *
   * @param data pointer to data
   * @param data_size size to write in bytes
   * @return std::size_t - bytes written
   */
  std::size_t write(const std::uint8_t *data, int data_size) const;

  /**
   * @brief read data from socket into buffer
   *
   * @param buffer pointer to buffer
   * @param buffer_size size to read in bytes
   * @return std::size_t - bytes read
   */
  std::size_t read(std::uint8_t *buffer, int buffer_size) const;

  /**
   * @brief Get the clients address
   *
   * @return  struct in_addr
   */
  struct in_addr getAddress();

private:
  /**
   * @brief free tls context and close socket
   * @note this function won't shutdown the tls session
   *
   */
  void close() noexcept;

  /** @brief socket descriptor  */
  int sd;

  /** @brief client address */
  struct in_addr address;

  /** @brief tls context  */
  SSL *ssl;
};
} // namespace W
