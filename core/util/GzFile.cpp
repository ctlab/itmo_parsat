#include "core/util/GzFile.h"

#include <glog/logging.h>

namespace core::util {

GzFile::GzFile(std::filesystem::path const& path) {
  CHECK_NOTNULL(file_ = gzopen(path.c_str(), "rb"));
}

GzFile::~GzFile() noexcept {
  if (file_ != nullptr) {
    gzclose(file_);
  }
}

gzFile GzFile::native_handle() noexcept {
  return file_;
}

}  // namespace core::util