#include <print>
#include <prisma/cli.hpp>

int main(int argc, char *argv[]) {
  PrismaCliConfig config;

  if (parse(argc, argv, config) != 0) {
    return 1;
  }

  if (config.command == "info") {
    std::println("Finding metadata for file: {}", config.file_in);
  } else {
    std::println(stderr, "invalid command");
    return 1;
  }

  return 0;
}
