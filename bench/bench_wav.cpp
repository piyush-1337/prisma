#include "utils.hpp"
#include <benchmark/benchmark.h>
#include <prisma/codecs/audio/wav.hpp>
#include <prisma/core/audio.hpp>

static void BM_WavDecode(benchmark::State &state) {
  auto file_data = read_file("../bench/assets/audio/sample.wav");
  if (file_data.empty()) {
    state.SkipWithError("Failed to load wav");
    return;
  }

  prisma::core::Audio audio;

  for (auto _ : state) {
    auto result = prisma::codec::wav::decode(file_data);
    if (!result) {
      state.SkipWithError(("Decode failed: " + result.error()).c_str());
      break;
    }
    audio = std::move(*result);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }

  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                          static_cast<int64_t>(audio.pcm.size()));
}

BENCHMARK(BM_WavDecode)->Unit(benchmark::kMillisecond);

static void BM_WavEncode(benchmark::State &state) {
  auto file_data = read_file("../bench/assets/audio/sample.wav");
  if (file_data.empty()) {
    state.SkipWithError("Failed to load wav");
    return;
  }

  auto decoded_res = prisma::codec::wav::decode(file_data);
  if (!decoded_res) {
    std::string err =
        "Failed to decode wav during setup: " + decoded_res.error();
    state.SkipWithError(err.c_str());
    return;
  }
  prisma::core::Audio audio = std::move(*decoded_res);

  for (auto _ : state) {
    auto result = prisma::codec::wav::encode(audio);
    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }

  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                          static_cast<int64_t>(audio.pcm.size()));
}

BENCHMARK(BM_WavEncode)->Unit(benchmark::kMillisecond);
