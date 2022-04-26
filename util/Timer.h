#ifndef ITMO_PARSAT_TIMER_H
#define ITMO_PARSAT_TIMER_H

#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <set>
#include <condition_variable>

#include "util/TimeTracer.h"

namespace util {

struct TimerHandleType {
  std::atomic_bool aborted{false};
  void abort() noexcept;
};

using TimerHandle = std::shared_ptr<TimerHandleType>;

struct TimerEvent {
  util::clock_t::time_point when;
  std::function<void()> callback;
  TimerHandle handle;
};

bool operator<(TimerEvent const& a, TimerEvent const& b) noexcept;

class Timer {
 public:
  Timer();

  ~Timer();

  TimerHandle add(
      std::function<void()> const& callback, util::clock_t::duration dur);

  template <typename F>
  auto launch(
      F&& f, std::function<void()> const& abort, util::clock_t::duration dur) {
    auto handle = add(abort, dur);
    auto result = f();
    handle->abort();
    return result;
  }

 private:
  bool _stop = false;
  std::set<TimerEvent> _events;
  std::condition_variable _cv;
  std::mutex _m;
  std::thread _thread;
};

}  // namespace util

#endif  // ITMO_PARSAT_TIMER_H
