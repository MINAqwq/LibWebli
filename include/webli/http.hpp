// Copyright 2024 Mina

#pragma once

#include <nlohmann/json.hpp>

#include <chrono>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

namespace W::Http {
/**
 * @brief HTTP Status Codes
 *
 */
enum class StatusCode {
  Continue = 100,
  SwitchingProtocol,
  Processing,
  EarlyHints,
  Ok = 200,
  Created,
  Accepted,
  NonAuthoritativeInformation,
  NoContent,
  ResetContent,
  PartialContent,
  MultiStatus,
  AlreadyReported,
  ImUsed = 226,
  MultipleChoices = 300,
  MovedPermanently,
  Found,
  SeeOther,
  NotModified,
  UseProxy,
  SwitchProxy,
  TemporaryRedirect,
  PermanentRedirect,
  BadRequest = 400,
  Unauthorized,
  PaymentRequired,
  Forbidden,
  NotFound,
  MethodNotAllowed,
  NotAcceptable,
  ProxyAuthenticationRequired,
  RequestTimeout,
  Conflict,
  Gone,
  LengthRequired,
  PreconditionFailed,
  PayloadTooLarge,
  UriTooLong,
  UnsupportedMediaType,
  RangeNotSatisfiable,
  ExpectationFailed,
  ImATeapot, // my personal fav
  MisdirectedRequest = 421,
  UnprocessableContent,
  Locked,
  FailedDependency,
  TooEarly,
  UpgradeRequired,
  PreconditionRequired = 428,
  TooManyRequests,
  RequestHeaderFieldsTooLarge = 431,
  InternalServerError = 500,
  NotImplemented,
  BadGateway,
  ServiceUnavailable,
  GatewayTimeout,
  HttpVersionNotSupported,
  VariantAlsoNegotiates,
  InsufficientStorage,
  LoopDetected,
  NotExtended = 510,
  NetworkAuthenticationRequired
};

namespace Header {
static constexpr const char *Cookie = "Cookie";
static constexpr const char *Connection = "Connection";
static constexpr const char *ContentLength = "Content-Length";
static constexpr const char *ContentType = "Content-Type";
static constexpr const char *Host = "Host";
static constexpr const char *SetCookie = "Set-Cookie";
static constexpr const char *UserAgent = "User-Agent";
} // namespace Header

namespace Cookie::SameSite {
static constexpr const char *Strict = "Strict";
static constexpr const char *Lax = "Lax";
static constexpr const char *None = "None";
} // namespace Cookie::SameSite

/**
 * @brief build a cookie string out of the given parameter.
 *
 * @param name cookie name
 * @param value cookie value
 * @param httpOnly http only attribute
 * @param domain domain scope
 * @param path path scope
 * @param expires expire date
 * @param same_site same site attribute
 * @return std::string
 */
std::string
buildCookieString(std::string_view name, std::string_view value,
                  bool httpOnly = false, std::string_view domain = "",
                  std::string_view path = "", std::string_view expires = "",
                  std::string_view same_site = Cookie::SameSite::Strict);

/**
 * @brief get the name of a numeric status code as string
 *
 * @param code http status code
 * @return std::string
 */
std::string StatusCodeToString(StatusCode code);

/**
 * @brief string hasher
 *
 */
struct StringHash {
  using hash_type = std::hash<std::string_view>;
  using is_transparent = void;

  std::size_t operator()(const char *str) const { return hash_type{}(str); }
  std::size_t operator()(std::string_view str) const {
    return hash_type{}(str);
  }
  std::size_t operator()(std::string const &str) const {
    return hash_type{}(str);
  }
};

/**
 * @brief Typedef for unordered string map with transparent custom hasher
 *
 */
using StringMap =
    std::unordered_map<std::string, std::string, StringHash, std::equal_to<>>;

/**
 * @brief get the position of the '?' in an http path
 *
 * @param path http path
 * @return std::size_t (std::string::npos if not found)
 */
std::size_t findGetParameter(std::string_view path) noexcept;

/**
 * @brief extract the get parameter from a path
 *
 * @param http_path
 * @return StringMap
 */
StringMap extractGetParameter(std::string_view http_path) noexcept;

/**
 * @brief extract the cookies from a cookie header string and return them as
 * string map
 *
 * @param cookie_string
 * @return StringMap
 */
StringMap extractCookies(const std::string_view &cookie_string) noexcept;

/**
 * @brief Base HTTP Object
 *
 */
class Object {
public:
  /**
   * @brief Construct a new empty HTTP Object
   *
   */
  Object() = default;

  /**
   * @brief Construct a new HTTP Object based on the parameter
   *
   * @param header http header
   * @param body http body
   * @param version http version
   */
  Object(const Http::StringMap &header, const std::string &body,
         const std::string &version = "HTTP/1.1");

  /**
   * @brief Destroy the HTTP Object
   *
   */
  virtual ~Object() = default;

  /**
   * @brief Get the HTTP header
   *
   * @param key name the value is saved under
   * @return std::string_view
   */
  std::string_view getHeader(const std::string &key) const noexcept;

  /**
   * @brief Get the HTTP body
   *
   * @return const std::string&
   */
  const std::string &getBody() const noexcept;

  /**
   * @brief Get the HTTP body as object.
   *
   * @return const nlohmann::json (empty on failure)
   */
  nlohmann::json getBodyJson() const noexcept;

  /**
   * @brief Get the HTTP version
   *
   * @return const std::string&
   */
  const std::string &getVersion() const noexcept;

  /**
   * @brief Set a value in the HTTP header
   *
   * @param key value name
   * @param value actual value
   */
  void setHeader(const std::string &key, const std::string &value) noexcept;

  /**
   * @brief Set the HTTP body and the Content-Length field
   *
   * @param data http body data (text only)
   */
  void setBody(const std::string &data) noexcept;

  /**
   * @brief Set the HTTP body, the Content-Length + Content-Type field
   *
   * @param json json data
   */
  void setBodyJson(const nlohmann::json &json) noexcept;

  /**
   * @brief Set the HTTP version
   *
   * @param version http version
   */
  void setVersion(const std::string &version) noexcept;

  /**
   * @brief build the http object to a string
   *
   * @return http object as std::string
   */
  virtual std::string build() const noexcept = 0;

protected:
  /**
   * @brief parse http header and then the body
   *
   * @param data http string stream
   */
  void parse(std::stringstream &data);

  /**
   * @brief read and parse the first line on the http stream
   *
   * @param data http string stream
   */
  virtual void parseFirstLine(std::stringstream &data) = 0;

  /**
   * @brief read and parse the http header from the stream
   *
   * @param data http string stream
   * @throws W::Exception on failure
   */
  void parseHeader(std::stringstream &data);

  /**
   * @brief read and parse the http body from the stream
   *
   * @param data http string stream
   */
  void parseBody(std::stringstream &data);

  /** @brief http header */
  StringMap header;

  /** @brief http body */
  std::string body;

  /** @brief http version */
  std::string version{"HTTP/1.1"};
};

/**
 * @brief HTTP Request
 *
 */
class Request : public Object {
public:
  /**
   * @brief Construct a new empty HTTP Request
   *
   */
  Request();

  /**
   * @brief Construct a new HTTP Request based on the parameter
   *
   * @param method http method
   * @param path path on server
   * @param header http header
   * @param body body data
   * @param version http version
   */
  Request(const std::string &method, const std::string &path,
          const Http::StringMap &header, const std::string &body,
          const std::string &version = "HTTP/1.1");

  /**
   * @brief Construct a new HTTP Request based on the input data
   *
   * @param data http string stream
   * @throws W::Exception on failure
   */
  explicit Request(std::stringstream &data);

  /**
   * @brief Destroy the Request object
   *
   */
  ~Request() override = default;

  /**
   * @brief Get the Request Method
   *
   * @return const std::string&
   */
  const std::string &getMethod() const noexcept;

  /**
   * @brief Get the Request Path
   *
   * @return const std::string&
   */
  const std::string &getPath() const noexcept;

  /**
   * @brief Get the http request cookies as string map
   *
   * @return StringMap
   */
  StringMap getCookies() const noexcept;

  /**
   * @brief Set the Request Method
   *
   * @param method http request method
   */
  void setMethod(const std::string &method) noexcept;

  /**
   * @brief Set the Request Path
   *
   * @param path http request path
   */
  void setPath(const std::string &path) noexcept;

  /**
   * @brief build the http request to a string
   *
   * @return http request as std::string
   */
  std::string build() const noexcept final;

private:
  /**
   * @brief read and parse the first line on the http stream
   *
   * @param data http string stream
   */
  void parseFirstLine(std::stringstream &data) final;

  /** @brief http request method */
  std::string method;

  /** @brief http request path */
  std::string path;
};

/**
 * @brief HTTP Response
 *
 */
class Response : public Object {
public:
  /**
   * @brief Construct a new HTTP Response
   *
   */
  Response();

  /**
   * @brief Construct a new HTTP Response based on the parameter
   *
   * @param status_code http status code
   * @param header http header
   * @param body http body
   * @param version http version
   */
  Response(StatusCode status_code, const Http::StringMap &header,
           const std::string &body, const std::string &version = "HTTP/1.1");

  /**
   * @brief Construct a new HTTP Response based on the input data
   *
   * @param data http string stream
   * @throws W::Exception on failure
   */
  explicit Response(std::stringstream &data);

  /**
   * @brief Destroy the Response object
   *
   */
  ~Response() override = default;

  /**
   * @brief Get the Response Status Code
   *
   * @return StatusCode
   */
  StatusCode getStatusCode() const noexcept;

  /**
   * @brief Set the Response Status Code
   *
   * @param code http status code
   */
  void setStatusCode(StatusCode code) noexcept;

  /**
   * @brief Set a session cookie with the given parameter. Assign an empty
   * string if you don't want to use them. Webli cookies are secure by default
   * because Webli does not support HTTP without TLS.
   *
   * @warning Because Webli uses a hashmap only 1 set-cookie operation is
   * supported atm :c
   *
   * @param name cookie name
   * @param value cookie value
   * @param httpOnly make cookie only usable in http requests
   * @param domain_scope domain scope
   * @param path_scope path scope
   * @param same_site same site attribute
   */
  void
  setCookie(std::string_view name, std::string_view value, bool httpOnly = true,
            std::string_view domain_scope = "",
            std::string_view path_scope = "",
            std::string_view same_site = Cookie::SameSite::Strict) noexcept;

  /**
   * @brief Set a cookie that expires with the given parameter. Assign an empty
   * string if you don't want to use them. Webli cookies are secure by default
   * because Webli does not support HTTP without TLS.
   *
   * @warning Because Webli uses a hashmap only 1 set-cookie operation is
   * supported atm :c
   *
   * @param name cookie name
   * @param value cookie value
   * @param expires date and time the cookie expires
   * @param httpOnly make cookie only usable in http requests
   * @param domain_scope domain scope
   * @param path_scope path scope
   * @param same_site same site attribute
   */
  void
  setCookie(std::string_view name, std::string_view value,
            std::chrono::time_point<std::chrono::system_clock> expires,
            bool httpOnly = true, std::string_view domain_scope = "",
            std::string_view path_scope = "",
            std::string_view same_site = Cookie::SameSite::Strict) noexcept;

  /**
   * @brief build the http response to a string
   *
   * @return http request as std::string
   */
  std::string build() const noexcept final;

private:
  /**
   * @brief read and parse the first line on the http stream
   *
   * @param data http string stream
   */
  void parseFirstLine(std::stringstream &data) final;

  /** @brief http status code */
  StatusCode status_code;
};

} // namespace W::Http
