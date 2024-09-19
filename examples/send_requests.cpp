/**
 * @file send_requests.cpp
 * @author mina (mina@minaqwq.dev)
 * @brief Example showing how to use the webclient api
 * @date 2024-09-11
 *
 * @copyright Copyright (c) 2024 Mina
 *
 * WebLi's webclient api can be used to send http requests to another https
 * server.
 */

#include <webli/exceptions.hpp>
#include <webli/http.hpp>
#include <webli/webclient.hpp>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

static const char *HTTP_METHOD_GET = "GET";

static const char *HTTP_USERAGENT = "WebLi";

int main() {
  W::HttpsClient client("example.com");
  try {

    //  send a synchronous get request
    auto res = client.send(HTTP_METHOD_GET, "/",
                           {{W::Http::Header::UserAgent, HTTP_USERAGENT}}, "");

    std::cerr << "[Sync] Request Status: "
              << std::to_string(static_cast<int>(res.getStatusCode())) << "\n";

    // send a asynchronous get request
    client.sendAsync(
        HTTP_METHOD_GET, "/", {{W::Http::Header::UserAgent, HTTP_USERAGENT}},
        "", [](const W::Http::Response &res) {
          std::cerr << "[Async] Request Status: "
                    << std::to_string(static_cast<int>(res.getStatusCode()))
                    << "\n";
          std::exit(0);
          return;
        });

    // wait until async request was send
    while (1) {
    }
    __builtin_unreachable();
  } catch (W::Exception &e) {
    std::cerr << "WebLi ERROR: " << e.getMessage() << "\n";
    return 1;
  } catch (std::exception &e) {
    std::cerr << "STL ERROR: " << e.what() << "\n";
    return 1;
  }
}
