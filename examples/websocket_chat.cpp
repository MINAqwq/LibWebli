/**
 * @file websocket_chat.cpp
 * @author mina (mina@minaqwq.dev)
 * @brief Example showing how to use weblis websockets
 * @date 2024-11-07
 *
 * @copyright Copyright (c) 2024 Mina
 *
 * WebLi's supports upgrading connections to secure websocket by throwing
 * `W::WebException::UpgradeToWebsocket`.
 *
 * This example shows how to use websockets on the client and server side to
 * create a super duper simple **non** secure live chat. DONT HOST MY EXAMPLE
 * GARBAGE PUBLIC!
 */

#include <webli/exceptions.hpp>
#include <webli/http.hpp>
#include <webli/router.hpp>
#include <webli/server.hpp>

#include <iostream>

static constexpr const char *g_html_chat = R"(
<!DOCTYPE html>
<html lang="en">
  <head>
    <title>Chat Example</title>
  </head>
  <body>
    <h1>WebliChat</h1>
    <div id="d_start">
      <form id="form_username">
        <input type="text" placeholder="username" />
        <input type="submit" value="start" />
      </form>
    </div>
    <div id="d_chat_head" hidden>
      <form id="form_chat">
        <input type="text" placeholder="message" />
        <input type="submit" value="send" />
      </form>
    </div>
    <div id="d_chat"></div>
    <script>
      let ws;

      function start_chating(event) {
        event.preventDefault();
        const form = event.currentTarget;

        ws = new WebSocket("wss://127.0.0.1:443/ws/chat?name=" + form[0].value);

        ws.onclose = (event) => {
          console.log("connection closed");
        };

        ws.onerror = (event) => {
          console.log("connection error");
        };

        ws.onmessage = (event) => {
          msg = JSON.parse(event.data);

          if (msg.type != "msg") {
            return;
          }

          let msg_elem = document.createElement("p");
          msg_elem.innerHTML = msg.author + ": " + msg.data;

          document.getElementById("d_chat").appendChild(msg_elem);
        };

        document.getElementById("d_start").hidden = true;
        document.getElementById("d_chat_head").hidden = false;

        document
          .getElementById("form_chat")
          .addEventListener("submit", send_msg);
      }

      function send_msg(event) {
        event.preventDefault();
        const form = event.currentTarget;

        ws.send(form[0].value);
      }

      document
        .getElementById("form_username")
        .addEventListener("submit", start_chating);
    </script>
  </body>
</html>
)";

class UserConnection {
public:
  UserConnection(std::string_view name,
                 const std::shared_ptr<W::WebsocketConnection> &ws)
      : name(name), ws(ws) {}

  ~UserConnection() = default;

  std::string name;
  std::shared_ptr<W::WebsocketConnection> ws;
};

static std::vector<UserConnection> g_current_online;

bool isNameLoggedIn(std::string_view name) {
  return std::ranges::find_if(g_current_online.begin(), g_current_online.end(),
                              [name](const UserConnection &con) {
                                return con.name == name;
                              }) != g_current_online.end();
}

int main() {
  W::Router router;

  router.get("/", [](const W::Http::Request &req,
                     std::shared_ptr<W::Http::Response> res) {
    res->setHeader(W::Http::Header::ContentType, "text/html");
    res->setBody("<h1>Websocket Example</h1><p>Go to `/chat` to get the full "
                 "experience of webli websocket :)</p>");
  });

  // websocket chat endpoint
  router.get("/ws/chat", [](const W::Http::Request &req,
                            std::shared_ptr<W::Http::Response> res) {
    const auto &params = W::Http::extractGetParameter(req.getPath());
    if (params.empty() || !params.contains("name")) {
      throw W::WebException::BadRequest();
    }

    const auto &name = params.at("name");

    if (isNameLoggedIn(name)) {
      throw W::WebException::Unauthorized();
    }

    std::string_view ws_key = req.getHeader(W::Http::Header::WsKey);
    std::string_view upgrade_key = req.getHeader(W::Http::Header::Upgrade);

    // if we find websocket handshake upgrade connection
    if (!ws_key.empty() && upgrade_key == "websocket") {
      throw W::WebException::UpgradeToWebsocket(
          ws_key,
          [name](W::WebsocketOpcode op, const std::vector<std::uint8_t> &data) {
            // this handler will get all text and binary messages, the vector it
            // returns contains frames the server will send back to the client

            auto msg_txt = std::string().assign(data.begin(), data.end());

            std::cerr << std::format("({}): {}\n", name, msg_txt);

            // prepare broadcast
            nlohmann::json answer_data;
            answer_data["type"] = "msg";
            answer_data["author"] = name;
            answer_data["data"] = msg_txt;

            W::WebsocketFrame frame;
            frame.setPayload(answer_data.dump());

            // send the message to the other clients
            for (auto &u : g_current_online) {
              u.ws->sendFrame(frame);
            }

            return std::vector<W::WebsocketFrame>{};
          },
          [name](const std::shared_ptr<W::WebsocketConnection> &ws) {
            // post handler gets called after the handshake was completed.
            // only if this handler returns true the connection stays open.
            std::cerr << std::format("({}) connected to chat\n", name);

            g_current_online.emplace_back(name, ws);
            return true;
          },
          [name]() {
            // when the server closes a websocket connection (reason dosn't
            // matter) it will call this function
            std::cerr << std::format("({}) disconnected from chat\n", name);

            const auto it = std::ranges::find_if(
                g_current_online.begin(), g_current_online.end(),
                [name](const UserConnection &con) { return con.name == name; });

            g_current_online.erase(it);
          });
    }

    res->setHeader(W::Http::Header::ContentType, "text/html");
    res->setBody("<!DOCTYPE html><html><body><p>you're not a "
                 "websocket!</p></body></html>");
  });

  router.get("/chat", [](const W::Http::Request &req,
                         std::shared_ptr<W::Http::Response> res) {
    res->setHeader(W::Http::Header::ContentType, "text/html");
    res->setBody(g_html_chat);
  });

  W::Server server{router};
  server.ssl_config("key.pem", "cert.pem");

  server.listen("127.0.0.1", 443);
}
