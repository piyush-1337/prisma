#pragma once

#include <string>

struct PrismaCliConfig {
    std::string command;
    std::string file_in;
};

int parse(int argc, char *argv[], PrismaCliConfig &config);
