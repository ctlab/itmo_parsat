#ifndef ITMO_PARSAT_SEMAPHORE_H
#define ITMO_PARSAT_SEMAPHORE_H

#include <cstdint>
#include <mutex>
#include <condition_variable>

#include "util/Logger.h"

namespace util {

struct Semaphore {
  void set_max(uint32_t max_value);

  void acquire(uint32_t num = 1);

  void release(uint32_t num = 1);

 private:
  std::mutex _mutex;
  std::condition_variable _cv;
  uint32_t _current = 0;

 public:
  uint32_t max;
};

}  // namespace util

#endif  // ITMO_PARSAT_SEMAPHORE_H
