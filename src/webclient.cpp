#include <webli/exceptions.hpp>
#include <webli/http.hpp>
#include <webli/smart_ptr/ssl.hpp>
#include <webli/webclient.hpp>

#include <memory>
#include <sstream>
#include <thread>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/tls1.h>
#include <openssl/types.h>

namespace W {

HttpsUrl parseUrl(std::string_view url_str) {
  HttpsUrl url;
  std::string url_copy{url_str};

  if (url_str.empty()) {
    throw Exception("Url is empty");
  }

  if (url_str.starts_with("http://")) {
    throw Exception("HTTP without TLS is not supported");
  }

  if (url_str.starts_with("https://")) {
    url_copy.erase(0, 8);
  }

  std::size_t path_start = url_copy.find('/');

  if (path_start != std::string::npos) {
    url.path = url_copy.substr(path_start, std::string::npos);
    url_copy.erase(path_start, std::string::npos);
  } else {
    url.path = "/";
  }

  url.hostname = url_copy;

  std::size_t port_start = url_copy.find(':');
  if (port_start != std::string::npos) {
    url.port = url_copy.substr(port_start + 1, path_start);
    url_copy.erase(port_start, path_start);
  } else {
    url.port = "443";
  }

  return url;
}

HttpsClient::HttpsClient(std::size_t buffer_size) : buffer_size(buffer_size) {}

HttpsClient::HttpsClient(std::string_view url_str, std::size_t buffer_size)
    : url(parseUrl(url_str)), buffer_size(buffer_size) {}

void HttpsClient::setUrl(const std::string &url_str) {
  this->url = parseUrl(url_str);
}

Http::Response HttpsClient::send(const std::string &method,
                                 const std::string &path,
                                 const Http::StringMap &header,
                                 const std::string &body) {
  Http::Request req{method, path, header, body};
  return this->send(req);
}

Http::Response HttpsClient::send(Http::Request req) {
  if (req.getHeader(Http::Header::Host).empty()) {
    req.setHeader(Http::Header::Host, this->url.hostname);
  }

  const SSL_METHOD *method = TLS_client_method();

  if (method == nullptr) {
    throw Exception("tls method is null");
  }

  std::unique_ptr<::SSL_CTX, Deleter::SSL_CTX> ctx{SSL_CTX_new(method)};

  if (ctx == nullptr) {
    throw Exception("ssl ctx is null");
  }

  SSL_CTX_set_options(ctx.get(), SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 |
                                     SSL_OP_NO_COMPRESSION);

  std::unique_ptr<::BIO, Deleter::WebBIO> web{BIO_new_ssl_connect(ctx.get())};

  BIO_set_conn_hostname(
      web.get(),
      std::format("{}:{}", this->url.hostname, this->url.port).c_str());

  SSL *ssl;
  BIO_get_ssl(web.get(), &ssl);

  static const char PREFERRED_CIPHERS[] =
      "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4";
  if (SSL_set_cipher_list(ssl, PREFERRED_CIPHERS) != 1) {
    throw Exception("PREFERRED_CIPHERS");
  }

  if (SSL_set_tlsext_host_name(ssl, url.hostname.c_str()) != 1) {
    throw Exception("SSL_set_tlsext_host_name");
  }

  if (BIO_do_connect(web.get()) != 1) {
    throw Exception("tls connect");
  }

  if (BIO_do_handshake(web.get()) != 1) {
    throw Exception("tls handshake");
  }
  BIO_puts(web.get(), req.build().c_str());

  std::string buffer;
  buffer.resize(this->buffer_size);

  std::size_t read = BIO_read(web.get(), buffer.data(), this->buffer_size);

  std::stringstream stream_data{};
  stream_data.write(buffer.c_str(), read);

  return Http::Response{stream_data};
}

void HttpsClient::sendAsync(
    const std::string &method, const std::string &path,
    const Http::StringMap &header, const std::string &body,
    std::function<void(const Http::Response &)> handler) {
  Http::Request req{method, path, header, body};
  this->sendAsync(req, handler);
}

void HttpsClient::sendAsync(
    const Http::Request &req,
    std::function<void(const Http::Response &)> handler) {
  std::jthread t{[this, req, handler]() { handler(this->send(req)); }};
  t.detach();
}
} // namespace W