// Copyright 2024 Mina

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace W::Storage {
std::string loadAsString(std::string_view path) noexcept;

std::vector<std::uint8_t> loadAsBinary(std::string_view path) noexcept;
} // namespace W::Storage