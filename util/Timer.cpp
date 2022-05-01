#include "util/Timer.h"

namespace util {

void TimerHandleType::abort() noexcept { aborted.store(true, std::memory_order_relaxed); }

bool operator<(TimerEvent const& a, TimerEvent const& b) noexcept { return a.when < b.when; }

Timer::Timer()
    : _thread([this] {
      while (!_stop.load(std::memory_order_relaxed)) {
        std::unique_lock<std::mutex> ul(_m);
        _cv.wait_for(
            ul, _events.begin()->when - util::clock_t::now(), [this] { return _stop.load(std::memory_order_relaxed); });
        if (IPS_UNLIKELY(_stop.load(std::memory_order_relaxed))) {
          break;
        }
        for (auto it = _events.begin(); it != _events.end() && it->when <= util::clock_t::now();) {
          if (!it->handle->aborted.load(std::memory_order_relaxed) && it->callback) {
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
  _stop.store(true, std::memory_order_relaxed);
  _cv.notify_one();
  if (_thread.joinable()) {
    _thread.join();
  }
}

TimerHandle Timer::add(std::function<void()> const& callback, util::clock_t::duration dur) {
  util::clock_t::time_point when = util::clock_t::now() + dur;
  auto handle = std::make_shared<TimerHandleType>();
  {
    std::lock_guard<std::mutex> lg(_m);
    _events.insert({when, callback, handle});
  }
  _cv.notify_one();
  return handle;
}

}  // namespace util
