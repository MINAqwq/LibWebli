#include <webli/exceptions.hpp>
#include <webli/router.hpp>

#include <string>

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

  std::string_view new_route = route;

  if (auto get_pos = Http::findGetParameter(new_route);
      get_pos != std::string::npos) {
    new_route.remove_suffix(new_route.size() - get_pos);
  }

  auto key = (std::string(method) + std::string(new_route));
  if (!this->map.contains(key)) {
    throw WebException::NotFound();
  }

  return this->map.at(key);
}
} // namespace W
