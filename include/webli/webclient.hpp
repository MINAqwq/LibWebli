// Copyright 2024 Mina

#pragma once

#include <webli/http.hpp>

#include <functional>
#include <openssl/ssl.h>
#include <string>

namespace W {

using HttpsUrl = struct HttpsUrl {
  std::string hostname;
  std::string port;
  std::string path;
};

HttpsUrl parseUrl(std::string_view url);

class HttpsClient {
public:
  HttpsClient(std::size_t buffer_size = 32768);
  HttpsClient(std::string_view url_str, std::size_t buffer_size = 32768);
  ~HttpsClient() = default;

  void setUrl(const std::string &url_str);

  Http::Response send(const std::string &method, const std::string &path,
                      const Http::StringMap &header, const std::string &body);

  Http::Response send(Http::Request req);

  void sendAsync(const std::string &method, const std::string &path,
                 const Http::StringMap &header, const std::string &body,
                 std::function<void(const Http::Response &resp)> handler);

  void sendAsync(const Http::Request &req,
                 std::function<void(const Http::Response &resp)> handler);

private:
  HttpsUrl url;
  std::size_t buffer_size;
};
} // namespace W