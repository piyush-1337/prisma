#pragma once

#include <cstdint>
#include <expected>
#include <prisma/cli.hpp>
#include <span>
#include <string>

namespace prisma {

std::expected<void, std::string> render_bmp(std::span<const uint8_t> file_data, Filters filters);

}
