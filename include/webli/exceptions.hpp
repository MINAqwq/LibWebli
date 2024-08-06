// Copyright 2024 Mina

#pragma once

#include <webli/http.hpp>

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
  HttpException(const Http::Response &resp) : resp(resp) {}

  Http::Response &getResponse() { return resp; }

private:
  Http::Response resp;
};

class NotFound : public HttpException {
public:
  NotFound()
      : HttpException(Http::Response(Http::StatusCode::NotFound, {},
                                     "<h1>Not Found</h1>")) {}
};
} // namespace Exceptions
} // namespace W
