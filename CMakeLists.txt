cmake_minimum_required(VERSION 3.16)

project(prisma)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER clang++)

set(SOURCES
    ./src/cli.cpp
    ./src/command.cpp
    ./src/file.cpp
    ./src/format.cpp
    ./src/commands/info.cpp
    ./src/commands/convert.cpp
    ./src/codecs/image/bmp/bmp.cpp
    ./src/codecs/image/png/png.cpp
    ./src/core/image.cpp
)

add_library(prisma_core OBJECT ${SOURCES})

target_include_directories(prisma_core PUBLIC ${PROJECT_SOURCE_DIR}/include)

find_package(ZLIB REQUIRED)
target_link_libraries(prisma_core PUBLIC ZLIB::ZLIB)

add_executable(prisma src/main.cpp)

find_package(CLI11 REQUIRED)
target_link_libraries(prisma PRIVATE prisma_core CLI11::CLI11)

include(GNUInstallDirs)
install(TARGETS prisma RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

option(PRISMA_BUILD_BENCH "build bench binary" OFF)

if(PRISMA_BUILD_BENCH)
    enable_testing()
    add_subdirectory(bench)
endif()
