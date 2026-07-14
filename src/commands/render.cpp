#include <cstddef>
#include <cstdint>
#include <print>
#include <prisma/commands/render.hpp>
#include <prisma/parser.hpp>
#include <prisma/process/bmp.hpp>

namespace prisma {

std::expected<void, std::string> render_bmp(std::span<const uint8_t> file_data,
                                            Filters filters) {
  return process_bmp(
      file_data, filters,
      [](uint8_t r, uint8_t g, uint8_t b, bool end_of_line, size_t index) {
        std::print("\x1b[48;2;{};{};{}m  \x1b[0m", +r, +g, +b);

        if (end_of_line) {
          std::println();
        }
      });
}

} // namespace prisma
