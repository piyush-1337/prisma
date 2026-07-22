find_package(benchmark REQUIRED)

add_executable(prisma_bench main.cpp bench_png.cpp bench_bmp.cpp bench_wav.cpp)

target_compile_options(prisma_bench PRIVATE -O3 -march=native)

target_link_libraries(prisma_bench 
    PRIVATE 
    prisma_core 
    benchmark::benchmark 
    benchmark::benchmark_main
)
