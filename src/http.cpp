#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <webli/exceptions.hpp>
#include <webli/http.hpp>

namespace W::Http {
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

void Object::parseBody(std::stringstream &data) { this->body = data.str(); }

Request::Request() : Object() {}
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

StatusCode Response::getStatusCode() { return this->status_code; }

std::string Response::build() const noexcept {
  std::string req;

  req += (this->version + " " +
          std::to_string(static_cast<int>(this->status_code)) + " " + "TODO" +
          "\r\n");

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
