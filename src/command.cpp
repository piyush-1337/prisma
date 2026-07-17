#include <print>
#include <prisma/cli.hpp>
#include <prisma/command.hpp>
#include <prisma/commands/convert.hpp>
#include <prisma/commands/info.hpp>
#include <prisma/file.hpp>
#include <prisma/format.hpp>
#include <prisma/parser.hpp>

namespace prisma {

std::expected<void, std::string> execute(PrismaCliConfig &config) {
  if (config.command == Command::NONE) {
    return {};
  }

  auto file = MappedFile::create(config.file_in);
  if (!file) {
    return std::unexpected("couldn't create file");
  }

  switch (config.command) {

  case Command::INFO: {
    auto type = identify_format(file->data());

    switch (type) {
    case Format::BMP: {
      return format::bmp::print_info(file->data());
    };
    case Format::WAV: {
      std::println("WAV");
      break;
    }
    case Format::FLAC: {
      std::println("FLAC");
      break;
    }
    case Format::PNG: {
      return format::png::print_info(file->data());
    }
    case Format::UNKNOWN: {
      std::println("UNKNOWN");
      break;
    };
    }
    break;
  }

  case Command::CONVERT: {
      return convert(file->data(), config.filters, config.file_out);
  }

  default: {
    return std::unexpected("should never reach here");
  }
  }

  return {};
}

} // namespace prisma
