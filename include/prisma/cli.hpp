#pragma once

#include <string>

namespace prisma {

enum class Command {
  INFO,
};

struct PrismaCliConfig {
  Command command;
  std::string file_in;
};

int parse(int argc, char *argv[], PrismaCliConfig &config);

} // namespace prisma
