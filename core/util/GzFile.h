#ifndef EVOL_GZFILE_H
#define EVOL_GZFILE_H

#include <zlib.h>
#include <filesystem>

#include "core/util/assert.h"

namespace core::util {

/**
 * GZ file wrapper utility.
 */
class GzFile {
 public:
  GzFile() = default;

  explicit GzFile(std::filesystem::path const& path);

  ~GzFile() noexcept;

  [[nodiscard]] gzFile native_handle() noexcept;

 private:
  gzFile file_ = nullptr;
};

}  // namespace core::util

#endif  // EVOL_GZFILE_H
