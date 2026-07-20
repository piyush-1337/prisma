#pragma once

#include <cstdint>
#include <vector>

namespace prisma::core {

struct Audio {
    /// sample rate frequency
    uint32_t sample_rate;
    /// mono (1), stereo (2)
    uint16_t channels;
    uint16_t bit_depth;
    std::vector<uint8_t> pcm;
};

} // namespace prisma::core
