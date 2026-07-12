#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <string_view>

namespace prisma {

class MappedFile {
public:
  static std::expected<MappedFile, std::string> create(std::string_view filepath);

  MappedFile(const MappedFile &) = delete;
  MappedFile &operator=(const MappedFile &) = delete;

  MappedFile(MappedFile &&) noexcept;
  MappedFile &operator=(MappedFile &&) noexcept;

  ~MappedFile();

private:
  uint8_t *m_ptr = nullptr;
  size_t m_size = 0;

  explicit MappedFile(uint8_t* ptr, size_t size) noexcept;
};

} // namespace prisma
