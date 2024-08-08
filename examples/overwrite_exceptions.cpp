/**
 * @file overwrite_exceptions.cpp
 * @author mina (mina@minaqwq.dev)
 * @brief Example showing how to overwrite exception responses.
 * @date 2024-08-08
 *
 * @copyright Copyright (c) 2024
 *
 * When you throw an `W::WebException::HttpException` it holds a response the
 * server will send to the client. You might want change a exceptions response
 * value. To do this you can just write to the responses static memory
 * locations.
 */

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