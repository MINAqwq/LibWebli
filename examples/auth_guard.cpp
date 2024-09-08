/**
 * @file auth_guard.cpp
 * @author mina (mina@minaqwq.dev)
 * @brief Example showing how to use multiple handler functions
 * @date 2024-08-08
 *
 * @copyright Copyright (c) 2024 mina
 *
 * This example defines 2 routes. `/` and `/admin`. `/` can be access by
 * everyone while `/admin` is guarded by the `isAdmin` subroutine.
 *
 * After the routes are build they are passed to the server. When someone sends
 * a `GET` to `/` or `/admin` the server will pass the request and a pointer to
 * the response buffer to all subroutines in the order we passed them to the
 * router until the last returned or one of them throws a object that inherited
 * from `W::WebException::HttpException`. In case a subroutine throws, the
 * response from the exception will be send to the client.
 */

#include <webli/con.hpp>
#include <webli/exceptions.hpp>
#include <webli/http.hpp>
#include <webli/router.hpp>
#include <webli/server.hpp>

void isAdmin(const W::Http::Request &req,
             std::shared_ptr<W::Http::Response> res) {
  auto token = req.getHeader("Token");
  if (token.empty() || token != "$1234%") {
    throw W::WebException::Unauthorized();
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
