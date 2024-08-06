// Copyright 2024 Mina

#pragma once

#include <webli/http.hpp>
#include <webli/storage.hpp>

#include <string_view>

// TODO: make http responses static content a user can overwrite to provide
// their own body

namespace W {
class Exception {
public:
  Exception(const char *error) : msg(error) {}
  const char *getMessage() const { return this->msg; }

private:
  const char *msg;
};

namespace WebException {
class HttpException {
public:
  HttpException() {}
  HttpException(const Http::Response &resp) : resp(resp) {}

  Http::Response &getResponse() { return resp; }

protected:
  Http::Response resp;
};

class NotFound : public HttpException {
public:
  NotFound() : HttpException(NotFound::response) {}

  static inline Http::Response response =
      Http::Response(Http::StatusCode::NotFound, {}, "<h1>Not Found</h1>");
};

class Unauthorized : public HttpException {
public:
  Unauthorized() : HttpException(Unauthorized::response) {}

  static inline Http::Response response = Http::Response(
      Http::StatusCode::Unauthorized, {}, "<h1>Unauthorized</h1>");
};

class FromStorage : public HttpException {
public:
  FromStorage(std::string_view path, const std::string &type,
              Http::StatusCode status_code = Http::StatusCode::Ok,
              std::unordered_map<std::string, std::string> header = {})
      : HttpException(
            Http::Response(status_code, header, Storage::loadAsString(path))) {
    this->resp.setHeader(Http::Header::ContentType, type);
  }
};

} // namespace WebException
} // namespace W
