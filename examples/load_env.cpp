/**
 * @file load_env.cpp
 * @author mina (mina@minaqwq.dev)
 * @brief Example showing how to use the dotenv loader
 * @date 2024-08-08
 *
 * @copyright Copyright (c) 2024
 *
 * `W::loadDotEnv` will search for a .env file in the current working directory.
 * If there is no, it will just return without doing something. You should
 * always have a fallback in case your variables are not loaded.
 */

#include <webli/dotenv.hpp>

#include <cstdlib>
#include <iostream>

int main() {
  W::loadDotEnv();

  const char *message = std::getenv("MSG");
  std::cerr << ((!message) ? "no message :c" : message) << "\n";
}
