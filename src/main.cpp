#include <print>
#include <prisma/command.hpp>

int main(int argc, char *argv[]) {
  prisma::PrismaCliConfig config;

  if (prisma::parse(argc, argv, config) != 0) {
    return 1;
  }

  auto result = prisma::execute(config);
  if (!result) {
    std::println(stderr, "Error: {}", result.error());
    return 1;
  }

  return 0;
}
