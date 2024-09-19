// Copyright 2024 Mina

#pragma once

#include <webli/http.hpp>

#include <functional>
#include <openssl/ssl.h>
#include <string>

namespace W {

/**
 * @brief Http URL
 *
 */
using HttpsUrl = struct HttpsUrl {
  /** @brief server hostname */
  std::string hostname;

  /** @brief server port */
  std::string port;

  /** @brief http path */
  std::string path;
};

/**
 * @brief parse a http url string to a HttpsUrl structure
 *
 * @param url http url string
 * @return HttpsUrl
 */
HttpsUrl parseUrl(std::string_view url);

/**
 * @brief HTTPS Client
 *
 */
class HttpsClient {
public:
  /**
   * @brief Construct a new Https Client object
   *
   * @param buffer_size response buffer size in bytes
   */
  HttpsClient(std::size_t buffer_size = 32768);

  /**
   * @brief Construct a new Https Client object
   *
   * @param url_str url string (https://example.com/)
   * @param buffer_size response buffer size in bytes
   */
  HttpsClient(std::string_view url_str, std::size_t buffer_size = 32768);

  /**
   * @brief Destroy the Https Client object
   *
   */
  ~HttpsClient() = default;

  /**
   * @brief Set the Url object
   *
   * @param url_str the new url used for opening connections
   */
  void setUrl(const std::string &url_str);

  /**
   * @brief Send a synchronous https request by providing the request fields
   *
   * @param method http method
   * @param path http path
   * @param header http header
   * @param body http body
   * @return Http::Response
   */
  Http::Response send(const std::string &method, const std::string &path,
                      const Http::StringMap &header, const std::string &body);

  /**
   * @brief Send a asynchronous https request by providing a http request
   *
   * @param req http request
   * @return Http::Response
   */
  Http::Response send(Http::Request req);

  /**
   * @brief Send a asynchronous https request by providing the request fields
   *
   * @param method http method
   * @param path http path
   * @param header http header
   * @param body http body
   * @param handler response handler
   */
  void sendAsync(const std::string &method, const std::string &path,
                 const Http::StringMap &header, const std::string &body,
                 std::function<void(const Http::Response &resp)> handler);

  /**
   * @brief Send a asynchronous https request by providing a http request
   *
   * @param req http request
   * @param handler response handler
   */
  void sendAsync(const Http::Request &req,
                 std::function<void(const Http::Response &resp)> handler);

private:
  /** @brief http url the client will use the open a connection */
  HttpsUrl url;

  /** @brief response buffer size in bytes */
  std::size_t buffer_size;
};
} // namespace W