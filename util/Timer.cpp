#include "util/Timer.h"

namespace util {

Timer::Timer()
    : _t([this] {
      while (!_stop) {
        std::unique_lock<std::mutex> ul(_m);
        _cv.wait(ul, [this] { return _do || _stop; });
        if (IPS_UNLIKELY(_stop)) {
          break;
        }
        if (IPS_UNLIKELY(!_do)) {
          continue;
        }

        _cv.wait_for(ul, _dur, [this] { return _stop || _abort; });
        if (!_abort && _callback) {
          _callback();
        }
        _do = false;
        _abort = false;
        _callback = {};
      }
    }) {}

Timer::~Timer() noexcept {
  {
    std::lock_guard<std::mutex> lg(_m);
    _abort = true;
    _stop = true;
  }
  _cv.notify_one();
  if (_t.joinable()) {
    _t.join();
  }
}

void Timer::abort() {
  {
    std::lock_guard<std::mutex> lg(_m);
    _abort = true;
    _callback = {};
  }
  _cv.notify_one();
}

void Timer::start(std::function<void()> const& f, core::clock_t::duration dur) {
  {
    std::lock_guard<std::mutex> lg(_m);
    _callback = f;
    _dur = dur;
    _do = true;
    _abort = false;
  }
  _cv.notify_one();
}

}  // namespace util