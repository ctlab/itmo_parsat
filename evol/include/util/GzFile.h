#ifndef EVOL_GZFILE_H
#define EVOL_GZFILE_H

#include <zlib.h>

#include <filesystem>

namespace ea::util {

class GzFile {
 public:
  GzFile() = default;

  GzFile(std::filesystem::path const& path);

  ~GzFile() noexcept;

  [[nodiscard]] gzFile native_handle() noexcept;

 private:
  gzFile file_ = nullptr;
};

}  // namespace ea::util

#endif  // EVOL_GZFILE_H
