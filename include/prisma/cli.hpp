#pragma once

#include <string>

namespace prisma {

enum class Command {
  NONE,
  INFO,
  RENDER,
};

struct Filters {
  bool invert;
  bool grayscale;
};

struct PrismaCliConfig {
  Command command = Command::NONE;
  std::string file_in;
  Filters filters;
};

int parse(int argc, char *argv[], PrismaCliConfig &config);

} // namespace prisma
