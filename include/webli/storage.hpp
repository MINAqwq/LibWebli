// Copyright 2024 Mina

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/**
 * @brief API for interacting with the filesystem
 *
 */
namespace W::Storage {
/**
 * @brief Load a file from disk and represent it as text.
 *
 * @param path virtual filesystem path
 * @return std::string
 */
std::string loadAsString(std::string_view path) noexcept;

/**
 * @brief Load a file from disk and represent it as raw bytes.
 *
 * @param path virtual filesystem path
 * @return std::vector
 */
std::vector<std::uint8_t> loadAsBinary(std::string_view path) noexcept;
} // namespace W::Storage