# Prisma

Prisma is a high-performance, from-scratch media transcoder written in C++26.

## Features

*   **From-Scratch Implementation**: Core decoding and encoding algorithms are implemented natively.
*   **SIMD Acceleration**: PNG decoding utilizes SIMD for vectorized spatial defiltering, achieving speeds exceeding 2 GB/s.
*   **Zero-Overhead Memory**: Designed to minimize allocations and prevent zero-initialization overhead during encoding.
*   **Media Filtering**: Built-in core manipulations, including Grayscale and Invert operations.

## Current Codec Support

### Images
*   **BMP**: Uncompressed bitmap support (Decode/Encode).
*   **PNG**: Lossless Deflate-based support (Decode/Encode) with SIMD-accelerated Up-filtering.

### Audio
*   **WAV**: Uncompressed PCM audio over RIFF container (Decode/Encode).
*   **MP3** (Planned): Lossy MPEG-1 Audio Layer III codec decoding.

## Build

### Using Nix
If you use [Nix](https://nixos.org/)

```bash
nix develop
```

This will instantly drop you into a shell with GCC 16, CMake, Ninja, libdeflate, and CLI11 perfectly configured.

### Manual Prerequisites
*   [CMake 3.16+](https://cmake.org/download/)
*   [Ninja Build System](https://ninja-build.org/)
*   [GCC 16+](https://gcc.gnu.org/) (Requires C++26)
*   [libdeflate](https://github.com/ebiggers/libdeflate)
*   [CLI11](https://github.com/CLIUtils/CLI11)
*   [Google Benchmark](https://github.com/google/benchmark) (Optional, for benchmarking)

### Compilation

```bash
mkdir build
cd build
cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Release
ninja
```

## Usage

### Run directly with Nix
If you have Nix installed, you can instantly run the latest version of Prisma directly from GitHub without cloning the repository or manually installing any dependencies:
```bash
nix run github:piyush-1337/prisma -- info <input_file>
```

### Inspect Media
Extracts and parses headers to provide information about the media format without fully decoding the payload.
```bash
./prisma info <input_file>
```

### Transcode Media
Converts media from one format to another, optionally applying spatial filters.
```bash
./prisma convert -i <input_file> -o <output_file> [options]
```

## Benchmarks

Run on an Intel i5-12450HX (8C/12T, 4.4 GHz) with `-O3 -march=native` (AVX2).

| Codec | Decode | Encode |
|-------|--------|--------|
| **PNG** | 2.30 GiB/s | 617 MiB/s |
| **BMP** | 4.05 GiB/s | 3.42 GiB/s |
| **WAV** | 13.04 GiB/s | 17.79 GiB/s |

```bash
cmake -B build -G Ninja -DPRISMA_BUILD_BENCH=ON
ninja -C build
cd build && ./bench/prisma_bench
```
