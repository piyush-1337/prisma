#pragma once

#include <string>

namespace prisma {

enum class Command {
  NONE,
  INFO,
  RENDER,
};

// always initialize your booleans
struct Filters {
  bool invert = false;
  bool grayscale = false;
};

struct PrismaCliConfig {
  Command command = Command::NONE;
  std::string file_in;
  Filters filters;
};

int parse(int argc, char *argv[], PrismaCliConfig &config);

} // namespace prisma
