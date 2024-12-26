/**
 * @file load_env.cpp
 * @author mina (mina@disappea.rs)
 * @brief Example showing how to use router groups
 * @date 2024-12-26
 *
 * @copyright Copyright (c) 2024
 *
 * `W::Router` can register routes and als sub `W::Router`. Ich a path begin
 * matches a group the request will be redirected to the sub router without the
 * group prefix.
 */

#include <webli/router.hpp>
#include <webli/server.hpp>

#include <cstdlib>

int main() {
  W::Router api_v1;
  W::Router api_v2;
  W::Router front;

  api_v1.get("/version", [](const W::Http::Request &req,
                            std::shared_ptr<W::Http::Response> res) {
    res->setBody("api v1");
  });

  api_v2.get("/version", [](const W::Http::Request &req,
                            std::shared_ptr<W::Http::Response> res) {
    res->setBody("api v2");
  });

  front.group("/v1", &api_v1);
  front.group("/v2", &api_v2);

  W::Server server{front};

  server.ssl_config("key.pem", "cert.pem");
  server.listen("127.0.0.1", 443);

  return 0;
}
