#include <print>
#include <prisma/cli.hpp>
#include <prisma/command.hpp>
#include <prisma/commands/info.hpp>
#include <prisma/commands/render.hpp>
#include <prisma/file.hpp>
#include <prisma/format.hpp>
#include <prisma/parser.hpp>

namespace prisma {

std::expected<void, std::string> execute(PrismaCliConfig &config) {
  if (config.command == Command::NONE) {
    return {};
  }

  auto file = prisma::MappedFile::create(config.file_in);
  if (!file) {
    return std::unexpected("couldn't create file");
  }

  switch (config.command) {

  case Command::INFO: {
    auto type = identify_format(file->data());

    switch (type) {
    case Format::BMP: {
      return print_bmp_info(file->data());
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
      std::println("PNG");
      break;
    }
    case Format::UNKNOWN: {
      std::println("UNKNOWN");
      break;
    };
    }
    break;
  }
  case Command::RENDER: {
    auto type = identify_format(file->data());

    switch (type) {
    case Format::BMP: {
      return render_bmp(file->data(), config.filters);
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
      std::println("PNG");
      break;
    }
    case Format::UNKNOWN: {
      std::println("UNKNOWN");
      break;
    };
    }
    break;
  }

  default: {
    return std::unexpected("should never reach here");
  }
  }

  return {};
}

} // namespace prisma
