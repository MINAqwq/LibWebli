// Copyright 2024 Mina

#pragma once

#include <webli/http.hpp>
#include <webli/storage.hpp>

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
  explicit Exception(const char *error) : msg(error) {}

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
 * @todo use sendfile
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

} // namespace WebException
} // namespace W
