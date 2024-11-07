// Copyright 2024 Mina

#pragma once

#include <functional>
#include <webli/con.hpp>

#include <vector>

namespace W {
/**
 * @brief Websocket magic string
 *
 */
static constexpr const char *WsMagic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

/**
 * @brief Websocket operations
 *
 */
enum class WebsocketOpcode : std::int8_t {
  None = -1,
  Continuation,
  Text,
  Binary,
  ConnectionCloseFrame = 8,
  Ping,
  Pong
};

/**
 * @brief Websocket Data Frame Header
 *
 */
class WebsocketFrameHeader {
public:
  /**
   * @brief Construct a new empy Websocket Frame Header
   *
   */
  WebsocketFrameHeader() = default;

  /**
   * @brief Construct a new Websocket Frame Header
   *
   * @param opcode ws operation code
   * @param masking_key 32bit masking key used for message xor masking
   * @param last_message last frame indicator
   * @param payload_length payload length
   */
  WebsocketFrameHeader(WebsocketOpcode opcode,
                       const std::array<std::uint8_t, 4> &masking_key,
                       bool last_message, std::uint64_t payload_length);

  /**
   * @brief return true if this frame is the last one (single frame message are
   * always the last one, because there is no fragmentation)
   *
   * @return true
   * @return false
   */
  bool isLastMessage() const noexcept;

  /**
   * @brief Get the Opcode
   *
   * @return WebsocketOpcode
   */
  WebsocketOpcode getOpcode() const noexcept;

  /**
   * @brief Get the Masking Key
   *
   * @return const std::array<std::uint8_t, 4>&
   */
  const std::array<std::uint8_t, 4> &getMaskingKey() const noexcept;

  /**
   * @brief return true if frame has error, check always after you build one
   * from a connection stream
   *
   * @return true
   * @return false
   */
  bool hasError() const noexcept;

  /**
   * @brief Get the Payload Length
   *
   * @return std::uint64_t
   */
  std::uint64_t getPayloadLength() const noexcept;

  /**
   * @brief build a frame from a connection stream (will hang until frame is in
   * memory or funny error)
   *
   * @param c connection stream
   * @return WebsocketFrameHeader
   */
  static WebsocketFrameHeader fromStream(const Con &c);

  /**
   * @brief deserialize the class to bytes you can send over the network
   *
   * @return std::vector<std::uint8_t>
   */
  std::vector<std::uint8_t> build() const noexcept;

  /**
   * @brief Set the Last Message indicator
   *
   * @param last_message
   */
  void setLastMessage(bool last_message) noexcept;

  /**
   * @brief Set the Opcode
   *
   * @param opcode
   */
  void setOpcode(WebsocketOpcode opcode) noexcept;

  /**
   * @brief Set the Masking Key
   *
   * @param masking_key
   */
  void setMaskingKey(std::array<std::uint8_t, 4> masking_key) noexcept;

  /**
   * @brief Set the Payload Length
   *
   * @param payload_length
   */
  void setPayloadLength(std::uint64_t payload_length) noexcept;

private:
  /** @brief error indicator (only for frames serielized by fromStream) */
  bool error{true};

  /** @brief last frame indicator (ws supports frame fragmentation) */
  bool last_message{true};

  /** @brief ws operation */
  WebsocketOpcode opcode{WebsocketOpcode::Text};

  /** @brief 32bit masking key */
  std::array<std::uint8_t, 4> masking_key{0, 0, 0, 0};

  /** @brief payload length in bytes */
  std::uint64_t payload_length{0};
};

/**
 * @brief complete websocket frame with header and payload
 *
 */
struct WebsocketFrame {
  WebsocketFrameHeader header;
  std::vector<std::uint8_t> payload;

  /**
   * @brief use the masking saved in the header to xor over the payload
   *
   */
  void mask();

  /**
   * @brief Set the Payload data + length and the operation to TEXT
   *
   * @param text text data
   */
  void setPayload(const std::string_view text);

  /**
   * @brief Set the Payload data + length and the operation to BINARY
   *
   * @param data non-text data
   */
  void setPayload(const std::vector<std::uint8_t> &data);
};

class WebsocketConnection;

/**
 * @brief WS Handler that's getting called on every user message the websocket
 * receives. The handler gets the operation code and the data in form of a byte
 * vector and has to return a vector containing websocket frames. The frames are
 * send in vector order back to the sender. If the vector is empty noting gets
 * send back.
 *
 */
using WebsocketHandler = std::function<std::vector<WebsocketFrame>(
    WebsocketOpcode, const std::vector<std::uint8_t> &)>;

/**
 * @brief Called after the WS handshake was sucessfull. Chance to obtain a
 * reference to a specific websocket connection and to stop the connection by
 * returning false.
 *
 */
using WebsocketPostHandshakeHandler =
    std::function<bool(const std::shared_ptr<WebsocketConnection> &con)>;

/**
 * @brief When the client or the server closes the socket the server will try to
 * call this handler after returning from the process subroutine.
 *
 */
using WebsocketCloseHandler = std::function<void()>;

/**
 * @brief WebSocket Connection (Version 13)
 *
 */
class WebsocketConnection {
public:
  /**
   * @brief Construct a new Websocket Connection
   *
   * @param con ref to secure tcp connection
   * @param handler generic ws message handler
   */
  explicit WebsocketConnection(const W::Con &con,
                               const WebsocketHandler &handler);
  /**
   * @brief Destroy the Websocket Connection
   *
   */
  ~WebsocketConnection();

  /**
   * @brief wait and return the next frame that was send to the server
   *
   * @return WebsocketFrame
   */
  WebsocketFrame readNextFrame() const;

  /**
   * @brief connection process routine
   *
   */
  void process();

  /**
   * @brief send a ws frame to the client
   *
   * @param frame ws frame
   */
  void sendFrame(const WebsocketFrame &frame) const;

  /**
   * @brief send multiple ws frames to the client by calling `sendFrame` in a
   * loop
   *
   * @param frames vector containing ws frames
   */
  void sendMultiple(const std::vector<WebsocketFrame> &frames) const;

  /**
   * @brief send a ping frame to the client
   *
   */
  void ping() const;

  /**
   * @brief send a pong frame to the client (for the rare case a client can send
   * pings)
   *
   * @param data ping data we send back
   *
   */
  void pong(const std::vector<std::uint8_t> &data) const;

  /**
   * @brief close the connection and send a CCF (CloseConnectionFrame) to the
   * client
   *
   */
  void close();

private:
  /** @brief generic message handler */
  WebsocketHandler handler;

  /** @brief ref to secure tcp connection */
  const W::Con &con;

  /** @brief current operation (to handle fregmentation) */
  WebsocketOpcode current_op = WebsocketOpcode::None;

  /** @brief connection state */
  bool connected{true};

  /** @brief current payload (to handle fregmentation) */
  std::vector<std::uint8_t> payload;
};
} // namespace W
