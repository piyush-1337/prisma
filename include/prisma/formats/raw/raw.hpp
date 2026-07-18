#pragma once

#include <cstdint>
#include <vector>

namespace prisma::format::raw {

struct RawImage {
    uint32_t width;
    uint32_t height;
    uint8_t channels;

    std::vector<uint8_t> pixels;
};

void apply_grayscale(RawImage &raw_image);
void apply_invert(RawImage &raw_image);

}
