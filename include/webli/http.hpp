// Copyright 2024 Mina

#pragma once

#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

namespace W::Http {
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

std::string StatusCodeToString(StatusCode code);

namespace Header {
static constexpr const char *Cookie = "Cookie";
static constexpr const char *Connection = "Connection";
static constexpr const char *ContentLength = "Content-Length";
static constexpr const char *ContentType = "Content-Type";
static constexpr const char *Host = "Host";
static constexpr const char *UserAgent = "User-Agent";
} // namespace Header

struct StringHash {
  using is_transparent = void;

  std::size_t operator()(std::string_view sv) const {
    std::hash<std::string_view> hasher;
    return hasher(sv);
  }
};

using StringMap =
    std::unordered_map<std::string, std::string, StringHash, std::equal_to<>>;

std::size_t findGetParameter(std::string_view path) noexcept;

StringMap extractGetParameter(std::string_view http_path) noexcept;

class Object {
public:
  Object() = default;
  Object(const Http::StringMap &header, const std::string &body,
         const std::string &version = "HTTP/1.1");
  virtual ~Object() = default;

  std::string_view getHeader(const std::string &key) const noexcept;

  const std::string &getBody() const noexcept;

  const std::string &getVersion() const noexcept;

  void setHeader(const std::string &key, const std::string &value) noexcept;

  void setBody(const std::string &data) noexcept;

  void setVersion(const std::string &version) noexcept;

  virtual std::string build() const noexcept = 0;

protected:
  void parse(std::stringstream &data);
  virtual void parseFirstLine(std::stringstream &data) = 0;
  void parseHeader(std::stringstream &data);
  void parseBody(const std::stringstream &data);

  StringMap header;
  std::string body;
  std::string version{"HTTP/1.1"};
};

class Request : public Object {
public:
  Request();
  explicit Request(std::stringstream &data);
  ~Request() override = default;

  const std::string &getMethod() const noexcept;

  const std::string &getPath() const noexcept;

  void setMethod(const std::string &method) noexcept;

  void setPath(const std::string &path) noexcept;

  std::string build() const noexcept final;

private:
  void parseFirstLine(std::stringstream &data) final;

  std::string method;
  std::string path;
};

class Response : public Object {
public:
  Response();
  Response(StatusCode status_code, Http::StringMap header,
           const std::string &body, const std::string &version = "HTTP/1.1");
  explicit Response(std::stringstream &data);
  ~Response() override = default;

  StatusCode getStatusCode() const noexcept;

  void setStatusCode(StatusCode code) noexcept;

  std::string build() const noexcept final;

private:
  void parseFirstLine(std::stringstream &data) final;

  StatusCode status_code;
};

} // namespace W::Http
