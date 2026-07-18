#include <prisma/formats/raw/raw.hpp>

namespace prisma::format::raw {

void apply_grayscale(RawImage &raw_image) {
  auto &pixels = raw_image.pixels;
  uint32_t total_pixels = raw_image.width * raw_image.height;

  for (uint32_t i{}; i < total_pixels; ++i) {
    size_t idx = i * raw_image.channels;
    uint8_t r = pixels[idx];
    uint8_t g = pixels[idx + 1];
    uint8_t b = pixels[idx + 2];

    uint8_t gray = static_cast<uint8_t>((r * 54 + g * 183 + b * 19) >> 8);

    pixels[idx] = gray;
    pixels[idx + 1] = gray;
    pixels[idx + 2] = gray;
  }
}

void apply_invert(RawImage &raw_image) {
  auto &pixels = raw_image.pixels;
  uint32_t total_pixels = raw_image.width * raw_image.height;

  for (uint32_t i{}; i < total_pixels; ++i) {
    size_t idx = i * raw_image.channels;

    pixels[idx] = 255 - pixels[idx];
    pixels[idx + 1] = 255 - pixels[idx + 1];
    pixels[idx + 2] = 255 - pixels[idx + 2];
  }
}

} // namespace prisma::format::raw
