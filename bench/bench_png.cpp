#include <benchmark/benchmark.h>
#include "utils.hpp"
#include <prisma/formats/raw/raw.hpp>
#include <prisma/formats/png/png.hpp>

static void BM_PngDecode(benchmark::State& state) {
    auto file_data = read_file("../bench/assets/png/generated_2048.png");
    if (file_data.empty()) {
        state.SkipWithError("Failed to load png");
        return;
    }

    prisma::format::raw::RawImage raw_image;

    for (auto _ : state) {
        auto result = prisma::format::png::decode(file_data, raw_image);
        if (!result) {
            state.SkipWithError(("Decode failed: " + result.error()).c_str());
            break;
        }
        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }
    
    int64_t pixel_bytes = raw_image.width * raw_image.height * raw_image.channels;
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * pixel_bytes);
}

BENCHMARK(BM_PngDecode)->Unit(benchmark::kMillisecond);

static void BM_PngEncode(benchmark::State& state) {
    auto file_data = read_file("../bench/assets/png/generated_2048.png");
    if (file_data.empty()) {
        state.SkipWithError("Failed to load png");
        return;
    }
    
    prisma::format::raw::RawImage raw_image;
    auto decoded_res = prisma::format::png::decode(file_data, raw_image);
    if (!decoded_res) {
        std::string err = "Failed to decode png during setup: " + decoded_res.error();
        state.SkipWithError(err.c_str());
        return;
    }
    
    for (auto _ : state) {
        auto result = prisma::format::png::encode(raw_image);
        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }
    
    int64_t pixel_bytes = raw_image.width * raw_image.height * raw_image.channels;
    state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * pixel_bytes);
}

BENCHMARK(BM_PngEncode)->Unit(benchmark::kMillisecond);
