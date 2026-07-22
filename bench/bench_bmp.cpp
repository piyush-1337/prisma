#include "utils.hpp"
#include <benchmark/benchmark.h>
#include <prisma/codecs/image/bmp.hpp>
#include <prisma/core/image.hpp>

static void BM_BmpDecode(benchmark::State &state) {
  auto file_data = read_file("../bench/assets/bmp/generated_2048.bmp");
  if (file_data.empty()) {
    state.SkipWithError("Failed to load bmp");
    return;
  }
  for (auto _ : state) {
    auto result = prisma::codec::bmp::decode(file_data);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }

  auto res = prisma::codec::bmp::decode(file_data);
  if (res) {
    int64_t pixel_bytes = res->width * res->height * res->channels;
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                            pixel_bytes);
  }
}

BENCHMARK(BM_BmpDecode)->Unit(benchmark::kMillisecond);

static void BM_BmpEncode(benchmark::State &state) {
  auto file_data = read_file("../bench/assets/bmp/generated_2048.bmp");
  if (file_data.empty()) {
    state.SkipWithError("Failed to load bmp");
    return;
  }
  auto decoded_res = prisma::codec::bmp::decode(file_data);
  if (!decoded_res) {
    state.SkipWithError("Failed to decode bmp");
    return;
  }
  for (auto _ : state) {
    auto result = prisma::codec::bmp::encode(*decoded_res);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }

  int64_t pixel_bytes =
      decoded_res->width * decoded_res->height * decoded_res->channels;
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                          pixel_bytes);
}

BENCHMARK(BM_BmpEncode)->Unit(benchmark::kMillisecond);
