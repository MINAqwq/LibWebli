// Copyright 2024 Mina

#include <webli/storage.hpp>

#include <filesystem>
#include <fstream>

namespace W::Storage {
std::string loadAsString(std::string_view path) noexcept {
  std::string buffer;
  std::size_t size;
  std::fstream stream{path.data(), std::ios::in};

  if (!stream.is_open()) {
    return "";
  }

  std::filesystem::path fspath(path);

  size = std::filesystem::file_size(fspath);
  buffer.resize(size);
  stream.read(buffer.data(), size);

  return buffer;
}
} // namespace W::Storage