#include <cstddef>
#include <cstdint>
#include <fstream>
#include <prisma/commands/convert.hpp>
#include <prisma/format.hpp>
#include <prisma/process/bmp.hpp>
#include <vector>

namespace prisma {

std::expected<void, std::string>
convert(std::span<const uint8_t> file_data, Filters filters,
        const std::filesystem::path &out_path) {

  std::vector<uint8_t> out_buffer(file_data.begin(), file_data.end());
  auto result = process_bmp(
      file_data, filters,
      [&](uint8_t r, uint8_t g, uint8_t b, bool end_of_line, size_t index) {
        out_buffer[index] = b;
        out_buffer[index + 1] = g;
        out_buffer[index + 2] = r;
      });

  if (!result) {
    return std::unexpected(result.error());
  }

  std::ofstream out_file(out_path, std::ios::binary);
  if (!out_file)
    return std::unexpected("Failed to open output file for writing");

  out_file.write(reinterpret_cast<const char *>(out_buffer.data()),
                 out_buffer.size());

  return {};
}

} // namespace prisma
