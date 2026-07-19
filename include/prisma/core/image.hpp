#pragma once

#include <cstdint>
#include <vector>

namespace prisma::core {

struct Image {
    uint32_t width;
    uint32_t height;
    uint8_t channels;

    std::vector<uint8_t> pixels;
};

void apply_grayscale(Image &image);
void apply_invert(Image &image);

}
