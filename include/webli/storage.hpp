// Copyright 2024 Mina

#pragma once

#include <string>
#include <string_view>

namespace W::Storage {
std::string loadAsString(std::string_view path) noexcept;
}