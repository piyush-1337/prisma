cmake_minimum_required(VERSION 3.16)

project(prisma)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/cli.cpp
    src/command.cpp
    src/file.cpp
    src/format.cpp
)

add_executable(prisma
    src/main.cpp
    ${SOURCES}
)

find_package(CLI11 REQUIRED)
target_link_libraries(prisma PRIVATE CLI11::CLI11)

target_include_directories(
    prisma
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

include(GNUInstallDirs)
install(TARGETS prisma
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
