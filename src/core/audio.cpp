#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <prisma/core/audio.hpp>

namespace prisma::core {

// hope this gets auto vectorized
void apply_volume(Audio &audio, const float multiplier) {
  if (audio.bit_depth == 16) {
    int16_t *samples = reinterpret_cast<int16_t *>(audio.pcm.data());
    size_t num_samples = audio.pcm.size() / 2;

    for (size_t i{}; i < num_samples; ++i) {
      float val = samples[i] * multiplier;
      val = std::clamp(val, -32768.0f, 32767.0f);
      samples[i] = static_cast<int16_t>(std::round(val));
    }
  } else if (audio.bit_depth == 8) {
    size_t num_samples = audio.pcm.size();

    for (size_t i{}; i < num_samples; ++i) {
      float val = static_cast<float>(audio.pcm[i]) - 128.0f;

      val = val * multiplier;
      val = val + 128.0f;
      val = std::clamp(val, 0.0f, 255.0f);

      audio.pcm[i] = static_cast<uint8_t>(std::round(val));
    }
  }
}

} // namespace prisma::core
