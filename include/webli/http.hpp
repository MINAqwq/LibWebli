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

class Object {
public:
  Object() = default;
  ~Object() = default;

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
  void parseBody(std::stringstream &data);

  std::unordered_map<std::string, std::string> header;
  std::string body;
  std::string version;
};

class Request : public Object {
public:
  Request();
  Request(std::stringstream &data);
  ~Request() = default;

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
  Response(std::stringstream &data);
  ~Response() = default;

  StatusCode getStatusCode() const noexcept;

  void setStatusCode(StatusCode code) noexcept;

  std::string build() const noexcept final;

private:
  void parseFirstLine(std::stringstream &data) final;

  StatusCode status_code;
};

} // namespace W::Http
