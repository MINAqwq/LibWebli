/**
 * @file load_from_storage.cpp
 * @author mina (mina@minaqwq.dev)
 * @brief Example showing how to load response data from a filesystem.
 * @date 2024-08-08
 *
 * @copyright Copyright (c) 2024 Mina
 *
 * As you may have noticed all responses are stored in memory.
 * But you can also easy load content from your file system with
 * `W::WebException::FromStorage`.
 *
 * Let's take the admin route example again, but instead of throwing
 * `W::WebException::Unauthorized` we load a html file from our filesystem.
 */

#include <webli/exceptions.hpp>
#include <webli/http.hpp>
#include <webli/router.hpp>
#include <webli/server.hpp>

namespace Path {
static constexpr const char *Unauthorized = "/example/err/unauthorized.html";
}

void isAdmin(const W::Http::Request &req,
             std::shared_ptr<W::Http::Response> res) {
  auto token = req.getHeader("Token");
  if (token.empty() || token != "$1234%") {
    throw W::WebException::FromStorage(Path::Unauthorized, "text/html");
  }
}

int main() {
  W::Router router;

  router.get("/", [](const W::Http::Request &req,
                     std::shared_ptr<W::Http::Response> res) {
    res->setHeader(W::Http::Header::ContentType, "text/html");
    res->setBody("<h1>Hallu</h1><p>Das ist eine coole seite</p>");
  });

  router.get("/admin",
             {{isAdmin},
              {[](const W::Http::Request &req,
                  std::shared_ptr<W::Http::Response> res) {
                res->setHeader(W::Http::Header::ContentType, "text/html");
                res->setBody(
                    "<h1>Hallu</h1><p>Das hier ist die Admin seite :)</p>");
              }}});

  W::Server server{router};
  server.ssl_config("key.pem", "cert.pem");

  server.listen("127.0.0.1", 443);
}