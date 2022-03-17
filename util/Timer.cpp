#include "util/Timer.h"

namespace util {

Timer::Timer()
    : _t([this] {
      while (!_stop) {
        {
          std::unique_lock<std::mutex> ul(_m);
          _cv.wait(ul, [this] { return _do || _stop; });
          if (IPS_UNLIKELY(_stop)) {
            break;
          }
        }

        std::unique_lock<std::mutex> ul(_m);
        _cv.wait_for(ul, _dur, [this] { return _stop || _abort; });
        if (!_abort) {
          _callback();
        }
        _do = false;
        _abort = false;
      }
    }) {}

Timer::~Timer() noexcept {
  {
    std::lock_guard<std::mutex> lg(_m);
    _abort = true;
    _stop = true;
  }
  _cv.notify_one();
  _t.join();
}

void Timer::abort() {
  {
    std::lock_guard<std::mutex> lg(_m);
    _abort = true;
  }
  _cv.notify_one();
}

void Timer::start(std::function<void()> const& f, core::clock_t::duration dur) {
  _callback = f;
  _dur = dur;
  {
    std::lock_guard<std::mutex> lg(_m);
    _do = true;
    _abort = false;
  }
  _cv.notify_one();
}

}  // namespace util