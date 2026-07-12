#include <CLI/CLI.hpp>
#include <prisma/cli.hpp>

int parse(int argc, char *argv[], PrismaCliConfig &config) {
  CLI::App app{"Prisma - Media transcoder"};
  app.require_subcommand(1);

  CLI::App *info_cmd =
      app.add_subcommand("info", "Display metadata of the file");
  std::string info_in;
  info_cmd->add_option("-i,--input", info_in, "Path to the input file");

  CLI11_PARSE(app, argc, argv);

  if (app.got_subcommand(info_cmd)) {
    config.command = "info";
    config.file_in = info_in;
  }

  return 0;
}
