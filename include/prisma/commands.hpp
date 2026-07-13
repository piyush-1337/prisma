#pragma once

#include <expected>
#include <prisma/cli.hpp>

namespace prisma {

std::expected<void, std::string> execute(PrismaCliConfig config);

} // namespace prisma
