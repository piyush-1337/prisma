#pragma once

#include <string>

namespace prisma {

enum class Command {
  NONE,
  INFO,
  CONVERT,
};

struct ImageFilters {
  bool invert = false;
  bool grayscale = false;
};

struct AudioFilers {
  float volume = 1.0;
};

// always initialize your booleans
struct Filters {
  ImageFilters image_filters;
  AudioFilers audio_filters;
};

struct PrismaCliConfig {
  Command command = Command::NONE;
  std::string file_in;
  std::string file_out;
  Filters filters;
};

int parse(int argc, char *argv[], PrismaCliConfig &config);

} // namespace prisma
