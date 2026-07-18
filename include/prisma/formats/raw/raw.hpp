#pragma once

#include <cstdint>
#include <vector>

namespace prisma::format::raw {

struct RawImage {
    uint32_t width;
    uint32_t height;

    std::vector<uint8_t> pixels;
};

}
