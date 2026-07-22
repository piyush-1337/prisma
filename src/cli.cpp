#include <CLI/CLI.hpp>
#include <prisma/cli.hpp>

namespace prisma {

int parse(int argc, char *argv[], prisma::PrismaCliConfig &config) {
  CLI::App app{"Prisma - Media transcoder"};
  app.require_subcommand(1);

  // info cmd
  CLI::App *info_cmd =
      app.add_subcommand("info", "Display metadata of the file");

  info_cmd->add_option("input", config.file_in, "Path to the input file")
      ->required()
      ->check(CLI::ExistingFile);
  info_cmd->callback([&]() { config.command = Command::INFO; });

  // convert cmd
  CLI::App *convert_cmd = app.add_subcommand(
      "convert", "Convert by either applying filters or chaning format");

  convert_cmd->add_option("-i,--input", config.file_in, "Path to input file")
      ->required()
      ->check(CLI::ExistingFile);
  convert_cmd->add_option("-o,--output", config.file_out, "Path to output file")
      ->required();
  convert_cmd->callback([&]() { config.command = Command::CONVERT; });

  convert_cmd->add_flag("--invert", config.filters.image_filters.invert,
                        "Invert all colors");
  convert_cmd->add_flag("--grayscale", config.filters.image_filters.grayscale,
                        "Apply luminance grayscale");
  convert_cmd->add_option("-v,--volume", config.filters.audio_filters.volume,
                          "Set volume");

  CLI11_PARSE(app, argc, argv);

  return 0;
}

} // namespace prisma
