#include "util/Semaphore.h"

namespace util {

void Semaphore::set_max(uint32_t max_value) { max = max_value; }

void Semaphore::acquire(uint32_t num) {
  IPS_VERIFY(num <= max);
  for (;;) {
    std::unique_lock<std::mutex> ul(_mutex);
    _cv.wait(ul, [this, num] { return _current + num <= max; });
    if (_current + num <= max) {
      _current += num;
      break;
    }
  }
}

void Semaphore::release(uint32_t num) {
  {
    std::lock_guard<std::mutex> lg(_mutex);
    _current -= num;
  }
  _cv.notify_all();
}

}  // namespace util