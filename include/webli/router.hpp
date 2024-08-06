// Copyright 2024 Mina

#pragma once

#include "webli/http.hpp"
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace W {
using HttpUserHandler =
    std::function<void(const Http::Request &, std::shared_ptr<Http::Response>)>;

class Router {
public:
  Router() = default;
  ~Router() = default;

  void get(std::string_view route, HttpUserHandler handler);
  void get(std::string_view route, const std::vector<HttpUserHandler> &handler);

  void post(std::string_view route, HttpUserHandler handler);
  void post(std::string_view route,
            const std::vector<HttpUserHandler> &handler);

  void put(std::string_view route, HttpUserHandler handler);
  void put(std::string_view route, const std::vector<HttpUserHandler> &handler);

  void patch(std::string_view route, HttpUserHandler handler);
  void patch(std::string_view route,
             const std::vector<HttpUserHandler> &handler);

  void del(std::string_view route, HttpUserHandler handler);
  void del(std::string_view route, const std::vector<HttpUserHandler> &handler);

  void custom(std::string_view method, std::string_view route,
              HttpUserHandler handler);
  void custom(std::string_view method, std::string_view route,
              const std::vector<HttpUserHandler> &handler);

  const std::vector<HttpUserHandler> &getHandler(std::string_view method,
                                                 std::string_view route) const;

private:
  std::unordered_map<std::string, std::vector<HttpUserHandler>> map;
};

} // namespace W
