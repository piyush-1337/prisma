#include <cstring>
#include <fcntl.h>
#include <prisma/file.hpp>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace prisma {

std::expected<MappedFile, std::string>
MappedFile::create(std::string_view filepath) {
  int fd = ::open(std::string(filepath).c_str(), O_RDONLY);
  if (fd == -1) {
    return std::unexpected("Failed to open the file: " +
                           std::string(std::strerror(errno)));
  }

  struct stat sb;
  if (::fstat(fd, &sb) == -1) {
    ::close(fd);
    return std::unexpected("Failed to read file stats: " +
                           std::string(std::strerror(errno)));
  }

  size_t size = sb.st_size;
  if (size == 0) {
    ::close(fd);
    return std::unexpected("File is empty");
  }

  void *ptr = ::mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (ptr == MAP_FAILED) {
    ::close(fd);
    return std::unexpected("Failed to map file into memory: " +
                           std::string(std::strerror(errno)));
  }

  ::close(fd);

  return MappedFile(static_cast<uint8_t *>(ptr), size);
}

std::span<const uint8_t> MappedFile::data() { return {m_ptr, m_size}; }

MappedFile::MappedFile(uint8_t *ptr, size_t size) noexcept
    : m_ptr(ptr), m_size(size) {}

MappedFile::MappedFile(MappedFile &&other) noexcept
    : m_ptr(other.m_ptr), m_size(other.m_size) {
  other.m_size = 0;
  other.m_ptr = nullptr;
}

MappedFile &MappedFile::operator=(MappedFile &&other) noexcept {
  if (this != &other) {
    if (m_ptr != nullptr) {
      ::munmap(m_ptr, m_size);
    }

    m_ptr = other.m_ptr;
    m_size = other.m_size;

    other.m_size = 0;
    other.m_ptr = nullptr;
  }

  return *this;
}

MappedFile::~MappedFile() {
  if (m_ptr != nullptr) {
    ::munmap(m_ptr, m_size);
  }
}

} // namespace prisma
