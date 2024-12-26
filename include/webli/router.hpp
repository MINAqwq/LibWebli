// Copyright 2024 Mina

#pragma once

#include <webli/con.hpp>
#include <webli/http.hpp>

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace W {
/**
 * @brief Typedef for user request handler function prototype
 *
 */
using HttpUserHandler =
    std::function<void(const Http::Request &, std::shared_ptr<Http::Response>)>;

/**
 * @brief HTTP Router
 *
 */
class Router {
public:
  /**
   * @brief Construct a new Router object
   *
   */
  Router() = default;

  /**
   * @brief Destroy the Router object
   *
   */
  ~Router() = default;

  /**
   * @brief register a new GET route with one handler
   *
   * @param route http route
   * @param handler user handler function
   */
  void get(std::string_view route, const HttpUserHandler &handler);

  /**
   * @brief register a new GET route with more than one handler
   *
   * @param route http route
   * @param handler vector containing user handler functions
   */
  void get(std::string_view route, const std::vector<HttpUserHandler> &handler);

  /**
   * @brief register a new POST route with one handler
   *
   * @param route http route
   * @param handler user handler function
   */
  void post(std::string_view route, const HttpUserHandler &handler);

  /**
   * @brief register a new POST route with more than one handler
   *
   * @param route http route
   * @param handler vector containing user handler functions
   */
  void post(std::string_view route,
            const std::vector<HttpUserHandler> &handler);

  /**
   * @brief register a new PUT route with one handler
   *
   * @param route http route
   * @param handler user handler function
   */
  void put(std::string_view route, const HttpUserHandler &handler);

  /**
   * @brief register a new PUT route with more than one handler
   *
   * @param route http route
   * @param handler vector containing user handler functions
   */
  void put(std::string_view route, const std::vector<HttpUserHandler> &handler);

  /**
   * @brief register a new PATCH route with one handler
   *
   * @param route http route
   * @param handler user handler function
   */
  void patch(std::string_view route, const HttpUserHandler &handler);

  /**
   * @brief register a new PATCH route with more than one handler
   *
   * @param route http route
   * @param handler vector containing user handler functions
   */
  void patch(std::string_view route,
             const std::vector<HttpUserHandler> &handler);

  /**
   * @brief register a new DELETE route with one handler
   *
   * @param route http route
   * @param handler user handler function
   */
  void del(std::string_view route, const HttpUserHandler &handler);

  /**
   * @brief register a new DELETE route with more than one handler
   *
   * @param route http route
   * @param handler vector containing user handler functions
   */
  void del(std::string_view route, const std::vector<HttpUserHandler> &handler);

  /**
   * @brief register a new route under a custom method with one handler
   *
   * @param method http method
   * @param route http route
   * @param handler vector containing user handler functions
   */
  void custom(std::string_view method, std::string_view route,
              const HttpUserHandler &handler);

  /**
   * @brief register a new route under a custom method with more than one
   * handler
   *
   * @param method http method
   * @param route http route
   * @param handler vector containing user handler functions
   */
  void custom(std::string_view method, std::string_view route,
              const std::vector<HttpUserHandler> &handler);

  /**
   * @brief register a group to redirect requests to another router
   *
   * E.g. when this router with the group "/v1" gets a request to "/v1/version",
   * the router registered under the group will get "/version" to process.
   *
   * Groups have priority over normal routes.
   *
   * From a performance perspective it's better to use router that only hold
   * groups and router that only hold routes. Avoid mixing them.
   *
   * @param route http route
   * @param router pointer to sub router
   */
  void group(std::string_view route, Router *router);

  /**
   * @brief Get the HttpUserHandler Vector registered under method +
   * route
   *
   * @param method http method
   * @param route http route
   * @return const std::vector<HttpUserHandler>&
   */
  const std::vector<HttpUserHandler> &getHandler(std::string_view method,
                                                 std::string_view route) const;

private:
  /** @brief hash map containing std::vector with HttpUserHandler */
  std::unordered_map<std::string, std::vector<HttpUserHandler>,
                     Http::StringHash, std::equal_to<>>
      map;

  /** @brief hash map containing router pointer */
  std::unordered_map<std::string, Router *, Http::StringHash, std::equal_to<>>
      groups;
};

} // namespace W
