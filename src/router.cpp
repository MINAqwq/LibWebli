#include "webli/exceptions.hpp"
#include <string>
#include <webli/router.hpp>

namespace W {
void Router::get(std::string_view route, HttpUserHandler handler) {
  this->custom("GET", route, handler);
}

void Router::get(std::string_view route,
                 const std::vector<HttpUserHandler> &handler) {
  this->custom("GET", route, handler);
}

void Router::post(std::string_view route, HttpUserHandler handler) {
  this->custom("POST", route, handler);
}

void Router::post(std::string_view route,
                  const std::vector<HttpUserHandler> &handler) {
  this->custom("POST", route, handler);
}

void Router::put(std::string_view route, HttpUserHandler handler) {
  this->custom("PUT", route, handler);
}
void Router::put(std::string_view route,
                 const std::vector<HttpUserHandler> &handler) {
  this->custom("PUT", route, handler);
}

void Router::patch(std::string_view route, HttpUserHandler handler) {
  this->custom("PATCH", route, handler);
}

void Router::patch(std::string_view route,
                   const std::vector<HttpUserHandler> &handler) {
  this->custom("PATCH", route, handler);
}

void Router::del(std::string_view route, HttpUserHandler handler) {
  this->custom("DELETE", route, handler);
}

void Router::del(std::string_view route,
                 const std::vector<HttpUserHandler> &handler) {
  this->custom("DELETE", route, handler);
}

void Router::custom(std::string_view method, std::string_view route,
                    HttpUserHandler handler) {
  this->map[std::string(method) + std::string(route)] = {handler};
}

void Router::custom(std::string_view method, std::string_view route,
                    const std::vector<HttpUserHandler> &handler) {
  this->map[std::string(method) + std::string(route)] = handler;
}

const std::vector<HttpUserHandler> &
Router::getHandler(std::string_view method, std::string_view route) const {
  auto key = (std::string(method) + std::string(route));
  if (!this->map.contains(key)) {
    throw WebException::NotFound();
  }

  return this->map.at(key);
}
} // namespace W
