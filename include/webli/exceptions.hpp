// Copyright 2024 Mina

#pragma once

namespace W {
class Exception {
public:
  Exception(const char *error) : msg(error) {}
  const char *getMessage() const { return this->msg; }

private:
  const char *msg;
};
} // namespace W
