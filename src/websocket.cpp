// Copyright 2024 Mina

#include <webli/exceptions.hpp>
#include <webli/websocket.hpp>

#include <array>
#include <bits/uintn-identity.h>
#include <byteswap.h>
#include <iostream>

#if __BYTE_ORDER == __BIG_ENDIAN
#define htonq(x) __uint64_identity(x)
#define ntohq(x) __uint64_identity(x)
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#define htonq(x) __bswap_64(x)
#define ntohq(x) __bswap_64(x)
#endif

namespace W {

WebsocketFrameHeader WebsocketFrameHeader::fromStream(const Con &c) {
  WebsocketFrameHeader frame;
  frame.error = true;

  std::uint8_t tmp;
  if (c.read(&tmp, sizeof(tmp)) != sizeof(tmp)) {
    return frame;
  }

  frame.last_message = tmp & 0x80;
  frame.opcode = static_cast<WebsocketOpcode>((tmp & 0xF));

  if (c.read(&tmp, sizeof(tmp)) != sizeof(tmp)) {
    return frame;
  }

  if (!(tmp & 0x80)) {
    return frame;
  }

  tmp &= 0x7F;
  if (tmp == 126) {
    std::uint16_t tmp2{0};
    if (c.read(reinterpret_cast<std::uint8_t *>(&tmp2), sizeof(tmp2)) !=
        sizeof(tmp2)) {
      return frame;
    }

    frame.payload_length = ntohs(tmp2);

  } else if (tmp == 127) {
    std::uint64_t tmp2{0};
    if (c.read(reinterpret_cast<std::uint8_t *>(&tmp2), sizeof(tmp2)) !=
        sizeof(tmp2)) {
      return frame;
    }

    frame.payload_length = ntohq(tmp2);
  } else {
    frame.payload_length = tmp;
  }

  if (c.read(reinterpret_cast<std::uint8_t *>(&frame.masking_key),
             sizeof(frame.masking_key)) != sizeof(frame.masking_key)) {
    return frame;
  }

  frame.error = false;
  return frame;
}

WebsocketFrameHeader::WebsocketFrameHeader(
    WebsocketOpcode opcode, const std::array<std::uint8_t, 4> &masking_key,
    bool last_message, std::uint64_t payload_length)
    : error(false), last_message(last_message), opcode(opcode),
      masking_key(masking_key), payload_length(payload_length) {}

bool WebsocketFrameHeader::isLastMessage() const noexcept {
  return this->last_message;
}

WebsocketOpcode WebsocketFrameHeader::getOpcode() const noexcept {
  return this->opcode;
}

const std::array<std::uint8_t, 4> &
WebsocketFrameHeader::getMaskingKey() const noexcept {
  return this->masking_key;
}

bool WebsocketFrameHeader::hasError() const noexcept { return this->error; }

std::uint64_t WebsocketFrameHeader::getPayloadLength() const noexcept {
  return this->payload_length;
}

std::vector<std::uint8_t> WebsocketFrameHeader::build() const noexcept {
  std::vector<std::uint8_t> data;
  data.resize(2);

  data[0] = 0x80 | (static_cast<std::uint8_t>(this->opcode));

  /* no masking */
  data[1] = 0x0;

  if (this->payload_length < 126) {
    data[1] |= (this->payload_length);
  } else if (this->payload_length <= 0xFFFF) {
    data[1] |= 126;
    data.resize(4);

    *reinterpret_cast<std::uint16_t *>(&data[2]) =
        htons(static_cast<std::uint16_t>(this->payload_length));

  } else {
    data[1] |= 128;
    data.resize(10);

    *reinterpret_cast<std::uint64_t *>(&data[2]) = htonq(this->payload_length);
  }

  return data;
}

void WebsocketFrameHeader::setLastMessage(bool last_message) noexcept {
  this->last_message = last_message;
}

void WebsocketFrameHeader::setOpcode(WebsocketOpcode opcode) noexcept {
  this->opcode = opcode;
}

void WebsocketFrameHeader::setMaskingKey(
    std::array<std::uint8_t, 4> masking_key) noexcept {
  this->masking_key = masking_key;
}

void WebsocketFrameHeader::setPayloadLength(
    std::uint64_t payload_length) noexcept {
  this->payload_length = payload_length;
}

void WebsocketFrame::mask() {
  const auto &key_arr = header.getMaskingKey();

  for (std::uint64_t i = 0; i < this->payload.size(); i++) {
    this->payload[i] ^= key_arr[i % 4];
  }
}

void WebsocketFrame::setPayload(const std::string_view text) {
  this->header.setOpcode(WebsocketOpcode::Text);
  this->header.setPayloadLength(text.size());
  this->payload.assign(text.begin(), text.end());
}

void WebsocketFrame::setPayload(const std::vector<std::uint8_t> &data) {
  this->header.setOpcode(WebsocketOpcode::Binary);
  this->header.setPayloadLength(data.size());
  this->payload = data;
}

WebsocketConnection::WebsocketConnection(const W::Con &con,
                                         const WebsocketHandler &handler)
    : handler(handler), con(con) {}

WebsocketConnection::~WebsocketConnection() {
  if (this->connected) {
    this->close();
  }
}

WebsocketFrame WebsocketConnection::readNextFrame() const {
  WebsocketFrame frame;

  frame.header = WebsocketFrameHeader::fromStream(this->con);
  if (frame.header.hasError()) {
    throw Exception("header with error");
  }

  std::uint64_t payload_length = frame.header.getPayloadLength();

  frame.payload.resize(payload_length);

  // funny bug, but let's be real, who sends more than 2GiB with a
  // single websocket message? haha
  if (con.read(frame.payload.data(), static_cast<int>(payload_length)) !=
      payload_length) {
    throw Exception("read != payload_length");
  }

  // "decrypt" bullshit
  frame.mask();

  return frame;
}

void WebsocketConnection::process() {
  try {
    while (this->connected) {
      auto frame = this->readNextFrame();

      switch (frame.header.getOpcode()) {
      case WebsocketOpcode::Continuation:
        break;

      case WebsocketOpcode::Text:
        this->current_op = WebsocketOpcode::Text;
        break;

      case WebsocketOpcode::Binary:
        this->current_op = WebsocketOpcode::Binary;
        break;

      case WebsocketOpcode::Ping:
        this->pong(frame.payload);
        continue;

      case WebsocketOpcode::Pong:
        continue;

      case WebsocketOpcode::ConnectionCloseFrame:
        this->connected = false;
        return;

      default:
        throw Exception("invalid opcode");
      }

      if (this->payload.empty() && frame.header.isLastMessage()) {
        auto resp = this->handler(this->current_op, frame.payload);
        this->sendMultiple(resp);
        continue;
      }

      this->payload.insert(this->payload.end(), frame.payload.begin(),
                           frame.payload.end());

      if (frame.header.isLastMessage()) {
        auto resp = this->handler(this->current_op, this->payload);
        this->sendMultiple(resp);

        this->payload.clear();
        this->current_op = WebsocketOpcode::None;
      }
    }
  } catch (const std::exception &e) {
    std::cerr << "ERROR: ws: " << e.what() << "\n";
    this->close();
    return;

  } catch (const Exception &e) {
    std::cerr << "ERROR: ws: " << e.getMessage() << "\n";
    this->close();
    return;
  }
}

void WebsocketConnection::sendFrame(const WebsocketFrame &frame) const {
  auto header_data = frame.header.build();

  // more funny haha
  this->con.write(header_data.data(), static_cast<int>(header_data.size()));
  this->con.write(frame.payload.data(), static_cast<int>(frame.payload.size()));
}

void WebsocketConnection::sendMultiple(
    const std::vector<WebsocketFrame> &frames) const {
  for (const auto &frame : frames) {
    this->sendFrame(frame);
  }
}

void WebsocketConnection::ping() const {
  static const std::vector<std::uint8_t> ping_payload = {0xFF, 0xFF, 0xFF,
                                                         0xFF};

  WebsocketFrame frame;
  frame.setPayload(ping_payload);

  frame.header.setLastMessage(true);
  frame.header.setOpcode(WebsocketOpcode::Ping);

  this->sendFrame(frame);
}

void WebsocketConnection::pong(const std::vector<std::uint8_t> &data) const {
  WebsocketFrame frame;
  frame.setPayload(data);

  frame.header.setLastMessage(true);
  frame.header.setOpcode(WebsocketOpcode::Pong);

  this->sendFrame(frame);
}

void WebsocketConnection::close() {
  WebsocketFrame frame;
  frame.header.setLastMessage(true);
  frame.header.setOpcode(WebsocketOpcode::ConnectionCloseFrame);

  this->sendFrame(frame);
  this->connected = false;
}

} // namespace W