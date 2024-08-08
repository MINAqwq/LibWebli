// Copyright 2024 Mina

namespace W {
/**
 * @brief search for a .env file int the current work directory and load it into
 * the environment.
 *
 * @param replace if set to true existing environment variables can be
 * overwritten
 *
 * @warning getline only reads the last line when it ends with a LF
 */
void loadDotEnv(bool replace = false);
} // namespace W
