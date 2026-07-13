#include <print>
#include <prisma/cli.hpp>
#include <prisma/commands.hpp>
#include <prisma/file.hpp>
#include <prisma/format.hpp>

namespace prisma {

std::expected<void, std::string> execute(PrismaCliConfig config) {
  if (config.command == Command::INFO) {
    auto file = prisma::MappedFile::create(config.file_in);
    if (!file) {
      return std::unexpected("couldn't create file");
    }
    auto type = identify_format(file->data());

    std::print("Found format: ");
    switch (type) {
    case Format::BMP: {
      std::println("BMP");
      break;
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
  } else {
    return std::unexpected("unknown command");
  }

  return {};
}

} // namespace prisma
