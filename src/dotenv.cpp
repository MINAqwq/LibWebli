#include <webli/dotenv.hpp>
#include <webli/storage.hpp>

#include <iostream>
#include <sstream>

namespace W {
bool hasSpace(std::string_view string) {
  return (string.find(' ') != std::string::npos) ||
         (string.find('\t') != std::string::npos);
}

void loadDotEnv(bool replace) {
  auto env_content = Storage::loadAsString(".env");
  if (env_content.empty()) {
    return;
  }

  std::cerr << "[WebLI] Loading .env\n";

  auto stream = std::istringstream(env_content);

  std::string line;

  auto print_error_and_exit = [](std::size_t line_no, const char *message) {
    std::cerr << "[WebLI] DotENV ERROR on line " << line_no << ": " << message
              << "\n";
    std::exit(1);
    __builtin_unreachable();
  };

  for (std::size_t line_no = 1; std::getline(stream, line).good(); line_no++) {
    if (line.empty()) {
      continue;
    }

    if (char line_start = line.at(0); line_start == '#') {
      continue;
    }

    std::size_t pos = line.find_first_of('=');
    if (pos == std::string::npos) {
      print_error_and_exit(line_no, "no assignment was made");
    }

    auto key = line.substr(0, pos);
    if (hasSpace(key)) {
      print_error_and_exit(line_no, "space in variable name");
    }

    auto value_size = line.size() - key.size() - 1;
    if (value_size <= 0) {
      print_error_and_exit(line_no, "assignment is empty");
    }

    auto value = line.substr(pos + 1, value_size);
    if (value.starts_with('"') && value.ends_with('"')) {
      value.resize(value.size() - 1);
      value.erase(0, 1);
    } else if (hasSpace(value)) {
      print_error_and_exit(line_no, "space in value");
    }

    setenv(key.c_str(), value.c_str(), replace);
  }
}
} // namespace W