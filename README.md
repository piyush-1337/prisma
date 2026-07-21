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

### Audio (Planned)
*   **WAV**: Uncompressed PCM audio over RIFF container.
*   **MP3**: Lossy MPEG-1 Audio Layer III codec decoding.

## Build

### Using Nix
If you use [Nix](https://nixos.org/)

```bash
nix develop
```

This will instantly drop you into a shell with GCC, CMake, Ninja, LIBDEFLATE, and CLI11 perfectly configured.

### Manual Prerequisites
*   [CMake 3.16+](https://cmake.org/download/)
*   [Ninja Build System](https://ninja-build.org/)
*   [GCC 14+](https://gcc.gnu.org/) (Requires C++26 standard support)
*   [LIBDEFLATE](https://github.com/ebiggers/libdeflate)
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

```bash
./bench/prisma_bench
```
