/**
 * @file json_get_echo.cpp
 * @author mina (mina@minaqwq.dev)
 * @brief Example showing how to to send a JSON response using nlohmann json.
 * @date 2024-08-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <webli/con.hpp>
#include <webli/http.hpp>
#include <webli/server.hpp>

int main() {
  W::Router router;

  router.get("/", [](const W::Http::Request &req,
                     std::shared_ptr<W::Http::Response> res) {
    nlohmann::json data;
    data["version"] = 1;
    data["server"] = "webli";

    res->setBodyJson(data);
  });

  router.get("/echo", [](const W::Http::Request &req,
                         std::shared_ptr<W::Http::Response> res) {
    nlohmann::json data;

    auto &get_field = data["get"];
    for (const auto &[k, v] : W::Http::extractGetParameter(req.getPath())) {
      get_field[k] = v;
    }

    res->setBodyJson(data);
  });

  W::Server server{router};
  server.ssl_config("key.pem", "cert.pem");

  server.listen("127.0.0.1", 443);
}