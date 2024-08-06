# LibWebli

This is the library powering the same named web framework (in work). LibWebli provides subroutines for hosting, connection handling, runtime exceptions and more. Made with the goal to make api programming in C++ less painful. But of course you can use it however you want.


## Dependencies

- Libc
- STL (C++20)
- BSD/Posix Sockets
- OpenSSL


## TODO

- [ ] JSON Integration (nlohmann?)
- [ ] HTTP Client API (like fetch in javascript)
- [ ] .env Parser/Loader


## Examples


### Server with 2 routes + auth guard

This example defines 2 routes. `/` and `/admin`. `/` can be access by everyone
while `/admin` is guarded by the `isAdmin` subroutine.

After the routes are build they are passed to the server. When someone sends a
`GET` to `/` or `/admin` the server will pass the request and a pointer to the
response buffer to all subroutines in the order we passed them to the router
until the last returned or one of them throws a object that inherited from
`W::WebException::HttpException`. In case a subroutine throws, the response
from the exception will be send to the client. 

```cpp
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
```


### Overwrite default exception responses

When you throw an `W::WebException::HttpException` it holds a response the
server will send to the client. I'ts most 

```cpp
#include <webli/exceptions.hpp>
#include <webli/http.hpp>
#include <webli/router.hpp>
#include <webli/server.hpp>

void setup() {
  W::WebException::NotFound::response = W::Http::Response(
      W::Http::StatusCode::NotFound, {}, "<h1>Overwritten Not Found hehe</h1>");
}

int main() {
  setup();

  // define routes and start server
}
```


### Load response content from storage

As you may have noticed all responses are stored in memory.
But you can also easy load content from your file system with
`W::WebException::FromStorage`.

Let's take the admin route example again, but instead of throwing
`W::WebException::Unauthorized` we load a html file from our file
system.
```cpp
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
```
