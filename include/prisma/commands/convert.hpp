#pragma once

#include <cstdint>
#include <expected>
#include <filesystem>
#include <prisma/cli.hpp>
#include <span>
#include <string>

namespace prisma {

std::expected<void, std::string> convert(std::span<const uint8_t> file_data,
                                         Filters filters,
                                         const std::filesystem::path &out_path);

}
