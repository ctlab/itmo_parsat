#include "util/Timer.h"

namespace util {

void TimerHandleType::abort() noexcept {
  aborted = true;
}

bool operator<(TimerEvent const& a, TimerEvent const& b) noexcept {
  return a.when < b.when;
}

Timer::Timer()
    : _thread([this] {
      while (!_stop) {
        std::unique_lock<std::mutex> ul(_m);
        _cv.wait_for(ul, _events.begin()->when - core::clock_t::now(), [this] {
          return _stop;
        });
        if (IPS_UNLIKELY(_stop)) {
          break;
        }
        for (auto it = _events.begin();
             it != _events.end() && it->when <= core::clock_t::now();) {
          if (!it->handle->aborted && it->callback) {
            try {
              it->callback();
            } catch (...) {
              // ignore
            }
          }
          it = _events.erase(it);
        }
      }
    }) {}

Timer::~Timer() {
  {
    std::lock_guard<std::mutex> lg(_m);
    _stop = true;
  }
  _cv.notify_one();
  if (_thread.joinable()) {
    _thread.join();
  }
}

TimerHandle Timer::add(
    std::function<void()> const& callback, core::clock_t::duration dur) {
  core::clock_t::time_point when = core::clock_t::now() + dur;
  auto handle = std::make_shared<TimerHandleType>();
  {
    std::lock_guard<std::mutex> lg(_m);
    _events.insert({when, callback, handle});
  }
  _cv.notify_one();
  return handle;
}

}  // namespace util