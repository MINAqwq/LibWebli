// Copyright 2024 Mina

#pragma once

#include <webli/con.hpp>
#include <webli/exceptions.hpp>
#include <webli/router.hpp>

#include <cstdint>
#include <openssl/ssl.h>
#include <string_view>

namespace W {
/**
 * @brief TLS over HTTP Server
 *
 */
class Server {
public:
  /**
   * @brief Construct a new Server object
   *
   * @param router Router containing path handler
   */
  explicit Server(const Router &router, std::size_t buffer_size = 2048);

  /**
   * @brief Destroy the Server object
   *
   */
  ~Server();

  /**
   * @brief Configure tls by passing key and cert path to the server.
   * @warning A call to this function is needed in order to call listen.
   *
   * @param key_path path to key file
   * @param cert_path path to cert file
   */
  void ssl_config(std::string_view key_path, std::string_view cert_path);

  /**
   * @brief Bind the server to the given interface and port and start accepting
   * new http connections.
   *
   * @param interface network interface ip
   * @param port tcp port
   */
  void listen(std::string_view interface, std::uint16_t port);

private:
  /**
   * @brief Internal subroutine used for new connection threads.
   *
   * @param client_sd
   * @param address
   * @param server
   */
  static void handle_con(int client_sd, struct in_addr address, Server *server);

  /**
   * @brief Internal subroutine used to upgrade a http request to a websocket
   * tunnel
   *
   * @param con
   * @param path
   * @param e
   */
  void handle_ws(const Con &con, std::string_view path,
                 WebException::UpgradeToWebsocket &e);

  /** @brief socket descriptor */
  int sd;

  /** @brief server first read buffer size */
  std::size_t buffer_size;

  /**
   * @brief byte that indicates that the server is running.
   * @todo turn false on interrupt
   */
  bool running{true};

  /** @brief Router object holding path handler */
  Router router;

  /** @brief SSL/TLS context */
  SSL_CTX *ctx;

  /** @brief print lock used to synchronize logging between threads */
  std::mutex print_lock;
};
} // namespace W
