#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <webli/exceptions.hpp>
#include <webli/http.hpp>

namespace W::Http {

std::string StatusCodeToString(StatusCode code) {
  switch (code) {
  case StatusCode::Continue:
    return "Continue";
  case StatusCode::SwitchingProtocol:
    return "Switching Protocol";
  case StatusCode::Processing:
    return "Processing";
  case StatusCode::EarlyHints:
    return "Early Hints";
  case StatusCode::Ok:
    return "OK";
  case StatusCode::Created:
    return "Created";
  case StatusCode::Accepted:
    return "Accepted";
  case StatusCode::NonAuthoritativeInformation:
    return "Non Authoritative Information";
  case StatusCode::NoContent:
    return "No Content";
  case StatusCode::ResetContent:
    return "Reset Content";
  case StatusCode::PartialContent:
    return "Partial Content";
  case StatusCode::MultiStatus:
    return "Multi-Status";
  case StatusCode::AlreadyReported:
    return "Already Reported";
  case StatusCode::ImUsed:
    return "Im Used";
  case StatusCode::MultipleChoices:
    return "Multiple Choices";
  case StatusCode::MovedPermanently:
    return "Moved Permanently";
  case StatusCode::Found:
    return "Found";
  case StatusCode::SeeOther:
    return "See Other";
  case StatusCode::NotModified:
    return "Not Modified";
  case StatusCode::UseProxy:
    return "Use Proxy";
  case StatusCode::SwitchProxy:
    return "Switch Proxy";
  case StatusCode::TemporaryRedirect:
    return "Temporary Redirect";
  case StatusCode::PermanentRedirect:
    return "Permanent Redirect";
  case StatusCode::BadRequest:
    return "Bad Request";
  case StatusCode::Unauthorized:
    return "Unauthorized";
  case StatusCode::PaymentRequired:
    return "Payment Required";
  case StatusCode::Forbidden:
    return "Forbidden";
  case StatusCode::NotFound:
    return "Not Found";
  case StatusCode::MethodNotAllowed:
    return "Method Not Allowed";
  case StatusCode::NotAcceptable:
    return "Not Acceptable";
  case StatusCode::ProxyAuthenticationRequired:
    return "Proxy Authentication Required";
  case StatusCode::RequestTimeout:
    return "Request Timeout";
  case StatusCode::Conflict:
    return "Conflict";
  case StatusCode::Gone:
    return "Gone";
  case StatusCode::LengthRequired:
    return "Length Required";
  case StatusCode::PreconditionFailed:
    return "Precondition Failed";
  case StatusCode::PayloadTooLarge:
    return "Payload Too Large";
  case StatusCode::UriTooLong:
    return "Uri Too Long";
  case StatusCode::UnsupportedMediaType:
    return "Unsupported Media Type";
  case StatusCode::RangeNotSatisfiable:
    return "Range Not Satisfiable";
  case StatusCode::ExpectationFailed:
    return "Expectation Failed";
  case StatusCode::ImATeapot:
    return "I'm A Teapot";
  case StatusCode::MisdirectedRequest:
    return "Misdirected Request";
  case StatusCode::UnprocessableContent:
    return "Unprocessable Content";
  case StatusCode::Locked:
    return "Locked";
  case StatusCode::FailedDependency:
    return "Failed Dependency";
  case StatusCode::TooEarly:
    return "Too Early";
  case StatusCode::UpgradeRequired:
    return "Upgrade Required";
  case StatusCode::PreconditionRequired:
    return "Precondition Required";
  case StatusCode::TooManyRequests:
    return "Too Many Requests";
  case StatusCode::RequestHeaderFieldsTooLarge:
    return "Request Header Fields Too Large";
  case StatusCode::InternalServerError:
    return "Internal Server Error";
  case StatusCode::NotImplemented:
    return "Not Implemented";
  case StatusCode::BadGateway:
    return "Bad Gateway";
  case StatusCode::ServiceUnavailable:
    return "Service Unavailable";
  case StatusCode::GatewayTimeout:
    return "Gateway Timeout";
  case StatusCode::HttpVersionNotSupported:
    return "Http Version Not Supported";
  case StatusCode::VariantAlsoNegotiates:
    return "Variant Also Negotiates";
  case StatusCode::InsufficientStorage:
    return "Insufficient Storage";
  case StatusCode::LoopDetected:
    return "Loop Detected";
  case StatusCode::NotExtended:
    return "Not Extended";
  case StatusCode::NetworkAuthenticationRequired:
    return "Network Authentication Required";
  default:
    return "";
  }
}

std::size_t findGetParameter(std::string_view path) noexcept {
  return path.find_first_of('?');
}

StringMap extractGetParameter(std::string_view path) noexcept {
  StringMap get_parameter;

  auto pos = findGetParameter(path);
  if (pos == std::string::npos) {
    return get_parameter;
  }
  pos++;

  path.remove_prefix(pos);

  while (!path.empty()) {
    auto param_end = path.find_first_of('=');
    if (param_end == std::string::npos || param_end == 0) {
      return {};
    }

    auto key = path.substr(0, param_end);

    ++param_end;
    if (param_end >= path.size()) {
      return {};
    }

    path.remove_prefix(param_end);

    auto value_end = path.find_first_of('&');
    if (value_end == std::string::npos) {
      get_parameter[std::string(key)] = path.substr(0, path.size());
      return get_parameter;
    }

    get_parameter[std::string(key)] = path.substr(0, value_end);
    path.remove_prefix(value_end + 1);
  }

  return get_parameter;
}

Object::Object(const Http::StringMap &header, const std::string &body,
               const std::string &version)
    : header(header), version(version) {
  // use setter to set content length
  this->setBody(body);
}

std::string_view Object::getHeader(const std::string &key) const noexcept {
  return this->header.contains(key) ? this->header.at(key)
                                    : std::string_view("");
}

const std::string &Object::getBody() const noexcept { return this->body; }

const std::string &Object::getVersion() const noexcept { return this->version; }

void Object::setHeader(const std::string &key,
                       const std::string &value) noexcept {
  this->header[key] = value;
}

void Object::setBody(const std::string &data) noexcept {
  (data.size() == 0)
      ? (void)this->header.erase(Header::ContentLength)
      : this->setHeader(Header::ContentLength, std::to_string(data.length()));

  this->body = data;
}

void Object::setVersion(const std::string &version) noexcept {
  this->version = version;
}

void Object::parse(std::stringstream &data) {
  this->parseHeader(data);
  this->parseBody(data);
}

void Object::parseHeader(std::stringstream &data) {
  std::string key;
  std::string value;
  std::string line;

  while (!data.bad()) {
    std::getline(data, line, '\r');
    data.get();
    if (line.empty()) {
      break;
    }

    auto pos = line.find_first_of(':');
    if (pos == std::string::npos || (pos + 2) >= line.size()) {
      throw Exception("malformed request");
    }

    key = line.substr(0, pos);
    value = line.substr(pos + 2);
    this->setHeader(key, value);
  }
}

void Object::parseBody(const std::stringstream &data) {
  this->body = data.str();
}

Request::Request() : Object() {}

Response::Response(StatusCode status_code, Http::StringMap header,
                   const std::string &body, const std::string &version)
    : Object(header, body, version), status_code(status_code) {}

Request::Request(std::stringstream &data) : Object() {
  this->parseFirstLine(data);
  this->parse(data);
}

const std::string &Request::getMethod() const noexcept { return this->method; }

const std::string &Request::getPath() const noexcept { return this->path; }

void Request::setMethod(const std::string &method) noexcept {
  this->method = method;
}

void Request::setPath(const std::string &path) noexcept { this->path = path; }

std::string Request::build() const noexcept {
  std::string req;

  req += (this->method + " " + this->path + " " + this->version + "\r\n");
  for (const auto &[k, v] : this->header) {
    req += (k + ": " + v + "\r\n");
  }

  req += ("\r\n" + body);

  return req;
}

void Request::parseFirstLine(std::stringstream &data) {
  static constexpr const std::size_t buffer_size{32};

  std::string buffer;
  std::size_t buffer_pos{0};
  std::size_t buffer_pos_old{0};

  std::getline(data, buffer, '\n');
  if (data.bad()) {
    throw Exception("malformed request");
  }

  buffer_pos = buffer.find_first_of(' ', buffer_pos_old);
  if (buffer_pos == std::string::npos) {
    throw Exception("malformed request");
  }

  this->method = buffer.substr(buffer_pos_old, buffer_pos);

  // skip whitespace
  buffer_pos_old = buffer_pos + 1;

  buffer_pos = buffer.find_first_of(' ', buffer_pos_old);
  if (buffer_pos == std::string::npos) {
    throw Exception("malformed request");
  }

  this->path = buffer.substr(buffer_pos_old, buffer_pos - buffer_pos_old);

  buffer_pos_old = buffer_pos + 1;

  buffer_pos = buffer.find_first_of('\r', buffer_pos_old);
  if (buffer_pos == std::string::npos) {
    throw Exception("malformed request");
  }

  this->version = buffer.substr(buffer_pos_old, buffer_pos - buffer_pos_old);
}

Response::Response() : Object() {}
Response::Response(std::stringstream &data) : Object() {
  this->parseFirstLine(data);
  this->parse(data);
}

StatusCode Response::getStatusCode() const noexcept {
  return this->status_code;
}

void Response::setStatusCode(StatusCode code) noexcept {
  this->status_code = code;
}

std::string Response::build() const noexcept {
  std::string req;

  req += (this->version + " " +
          std::to_string(static_cast<int>(this->status_code)) + " " +
          StatusCodeToString(this->status_code) + "\r\n");

  for (const auto &[k, v] : this->header) {
    req += (k + ": " + v + "\r\n");
  }

  req += ("\r\n" + body);

  return req;
}

void Response::parseFirstLine(std::stringstream &data) {
  static constexpr const std::size_t buffer_size{32};

  std::string buffer;
  std::size_t buffer_pos{0};
  std::size_t buffer_pos_old{0};

  std::getline(data, buffer, '\n');
  if (data.bad()) {
    throw Exception("malformed request");
  }

  buffer_pos = buffer.find_first_of(' ', buffer_pos_old);
  if (buffer_pos == std::string::npos) {
    throw Exception("malformed request");
  }

  this->version = buffer.substr(buffer_pos_old, buffer_pos);

  // skip whitespace
  buffer_pos_old = buffer_pos + 1;

  buffer_pos = buffer.find_first_of(' ', buffer_pos_old);
  if (buffer_pos == std::string::npos) {
    throw Exception("malformed request");
  }

  this->status_code = static_cast<StatusCode>(std::strtol(
      buffer.substr(buffer_pos_old, buffer_pos - buffer_pos_old).c_str(),
      nullptr, 10));
  if (status_code == static_cast<StatusCode>(0)) {
    throw Exception("malformed request");
  }

  buffer_pos_old = buffer_pos + 1;

  buffer_pos = buffer.find_first_of('\r', buffer_pos_old);
  if (buffer_pos == std::string::npos) {
    throw Exception("malformed request");
  }

  auto status_code_name =
      buffer.substr(buffer_pos_old, buffer_pos - buffer_pos_old);
}

} // namespace W::Http
