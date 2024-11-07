// Copyright 2024 Mina

#pragma once

#include <iostream>
#include <webli/http.hpp>
#include <webli/storage.hpp>
#include <webli/websocket.hpp>

#include <base64.hpp>

#include <string_view>

namespace W {
/**
 * @brief Generic Exception holding a message to display
 *
 */
class Exception {
public:
  /**
   * @brief Construct a new Exception object
   *
   * @param error pointer to error message
   */
  explicit Exception(const char *error) : msg(error) {
    std::cerr << error << "\n";
  }

  /**
   * @brief Get the Message object
   *
   * @return const char* - pointer to error message
   */
  const char *getMessage() const { return this->msg; }

private:
  /** @brief pointer to error message */
  const char *msg;
};

namespace WebException {
/**
 * @brief Generic HTTP Exception used as base for the specific ones
 *
 */
class HttpException {
public:
  /**
   * @brief Construct a new Http Exception object
   *
   */
  HttpException() = default;

  /**
   * @brief Construct a new Http Exception object
   *
   * @param resp http response to return
   */
  explicit HttpException(const Http::Response &resp) : resp(resp) {}

  /**
   * @brief Get the Response object
   *
   * @return Http::Response&
   */
  Http::Response &getResponse() { return resp; }

protected:
  /** @brief http response the server will send the client on throw */
  Http::Response resp;
};

/**
 * @brief Exception holding a 404 Not Found
 *
 */
class NotFound : public HttpException {
public:
  /**
   * @brief Construct a new Not Found object
   *
   */
  NotFound() : HttpException(NotFound::response) {}

  /**
   * @brief static response buffer, can be overwritten by user
   *
   */
  static inline Http::Response response =
      Http::Response(Http::StatusCode::NotFound, {}, "<h1>Not Found</h1>");
};

/**
 * @brief Exception holding a 400 Bad Request
 *
 */
class BadRequest : public HttpException {
public:
  BadRequest() : HttpException(BadRequest::response) {}

  /**
   * @brief static response buffer, can be overwritten by user
   *
   */
  static inline Http::Response response =
      Http::Response(Http::StatusCode::BadRequest, {}, "<h1>Bad Request</h1>");
};

/**
 * @brief Exception holding a 401 Unauthorized
 *
 */
class Unauthorized : public HttpException {
public:
  Unauthorized() : HttpException(Unauthorized::response) {}

  /**
   * @brief static response buffer, can be overwritten by user
   *
   */
  static inline Http::Response response = Http::Response(
      Http::StatusCode::Unauthorized, {}, "<h1>Unauthorized</h1>");
};

/**
 * @brief Exception loading a response body from storage
 *
 */
class FromStorage : public HttpException {
public:
  /**
   * @brief Construct a new From Storage object
   *
   * @param path path to file in filesystem
   * @param type http file type (eg. text/html or application/json)
   * @param status_code http status code
   * @param header http header
   */
  FromStorage(std::string_view path, const std::string &type,
              Http::StatusCode status_code = Http::StatusCode::Ok,
              const Http::StringMap &header = {})
      : HttpException(
            Http::Response(status_code, header, Storage::loadAsString(path))) {
    this->resp.setHeader(Http::Header::ContentType, type);
  }
};

/**
 * @brief Upgrade the current http connection to a websocket connection. Only
 * for clients that send a websocket key.
 *
 */
class UpgradeToWebsocket : public HttpException {
public:
  explicit UpgradeToWebsocket(
      std::string_view key, const WebsocketHandler &handler,
      const WebsocketPostHandshakeHandler &post_handler = nullptr,
      const WebsocketCloseHandler &close_handler = nullptr)
      : HttpException(UpgradeToWebsocket::handhshake), handler(handler),
        post_handler(post_handler), close_handler(close_handler) {
    this->resp.setHeader(Http::Header::WsAccept, accept_key(key));
  }

  const WebsocketHandler &getHandler() const { return this->handler; }

  const WebsocketPostHandshakeHandler &getPostHandler() const {
    return this->post_handler;
  }

  const WebsocketCloseHandler &getCloseHandler() const {
    return this->close_handler;
  }

private:
  /**
   * @brief static response buffer, can be overwritten by user
   *
   */
  static inline Http::Response handhshake =
      Http::Response(Http::StatusCode::SwitchingProtocol,
                     {{Http::Header::Upgrade, "websocket"},
                      {Http::Header::Connection, "Upgrade"}},
                     "");

  static std::string accept_key(std::string_view key) {
    std::string buffer;
    buffer = key;
    buffer += WsMagic;

    std::array<std::uint8_t, SHA_DIGEST_LENGTH> hash;
    hash.fill(0);

    SHA1(reinterpret_cast<std::uint8_t *>(buffer.data()), buffer.size(),
         hash.data());

    return base64::encode_into<std::string>(hash.begin(), hash.end());
  }

  WebsocketHandler handler;
  WebsocketPostHandshakeHandler post_handler;
  WebsocketCloseHandler close_handler;
};

} // namespace WebException
} // namespace W
