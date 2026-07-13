#include <CLI/CLI.hpp>
#include <prisma/cli.hpp>

namespace prisma {

int parse(int argc, char *argv[], prisma::PrismaCliConfig &config) {
  CLI::App app{"Prisma - Media transcoder"};
  app.require_subcommand(1);

  CLI::App *info_cmd =
      app.add_subcommand("info", "Display metadata of the file");
  std::string info_in;
  info_cmd->add_option("-i,--input", info_in, "Path to the input file")->required();

  CLI11_PARSE(app, argc, argv);

  if (app.got_subcommand(info_cmd)) {
    config.command = prisma::Command::INFO;
    config.file_in = info_in;
  }

  return 0;
}

} // namespace prisma
