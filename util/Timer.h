#ifndef ITMO_PARSAT_TIMER_H
#define ITMO_PARSAT_TIMER_H

#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "util/TimeTracer.h"

namespace util {

class Timer {
 public:
  Timer();

  ~Timer() noexcept;

  void start(std::function<void()> const& f, core::clock_t::duration dur);

  void abort();

  template <typename R, typename F>
  R launch(
      F&& runnable, std::function<void()> const& interrupt,
      core::clock_t::duration dur);

 private:
  bool _stop = false;
  bool _do = false;
  bool _abort = false;
  std::function<void()> _callback;
  core::clock_t::duration _dur{};
  std::mutex _m;
  std::condition_variable _cv;
  std::thread _t;
};

template <typename R, typename F>
R Timer::launch(
    F&& runnable, const std::function<void()>& interrupt,
    core::clock_t::duration dur) {
  start(interrupt, dur);
  R val = runnable();
  abort();
  return val;
}

}  // namespace util

#endif  // ITMO_PARSAT_TIMER_H
