#pragma once

#include <cstdint>
#include <expected>
#include <filesystem>
#include <prisma/cli.hpp>
#include <prisma/format.hpp>
#include <span>
#include <string>

namespace prisma {

std::expected<void, std::string> convert(std::span<const uint8_t> file_data,
                                         Filters filters,
                                         const std::filesystem::path &out_path);

std::expected<void, std::string>
convert_image(Format src, Format dest, std::span<const uint8_t> file_data,
              ImageFilters image_filters, const std::filesystem::path &out_path);

std::expected<void, std::string>
convert_audio(Format src, Format dest, std::span<const uint8_t> file_data,
              AudioFilers audio_filters, const std::filesystem::path &out_path);

} // namespace prisma
