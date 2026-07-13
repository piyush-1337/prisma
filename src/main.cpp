#include <print>
#include <prisma/cli.hpp>
#include <prisma/commands.hpp>
#include <prisma/file.hpp>
#include <prisma/format.hpp>

int main(int argc, char *argv[]) {
  prisma::PrismaCliConfig config;

  if (parse(argc, argv, config) != 0) {
    return 1;
  }

  auto result = prisma::execute(config);
  if (!result) {
    std::println(stderr, "Error: {}", result.error());
    return 1;
  }

  return 0;
}
