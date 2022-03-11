#ifndef ITMO_PARSAT_EXECUTION_H
#define ITMO_PARSAT_EXECUTION_H

#include <cstdint>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <filesystem>

#include <glog/logging.h>
#include <boost/process.hpp>

#include "util/TimeTracer.h"

namespace infra::execution {

class Execution {
 private:
  static uint64_t _timestamp() noexcept {
    return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
  }

 public:
  // clang-format off
  using callback_t = std::function<void(
      uint64_t,  // started_at
      uint64_t,  // finished_at
      int,       // exit code
      bool,      // is interrupted?
      bool       // time_limit is exceeded?
  )>;
  // clang-format on

 public:
  template <typename... Args, typename Stderr, typename Stdout>
  explicit Execution(
      callback_t const& callback, Stderr d_stderr, Stdout d_stdout, uint64_t time_limit_s,
      Args const&... args) {
    std::stringstream launch;
    ((launch << ' ' << args), ...);
    IPS_INFO("Started execution:" << launch.str());

    _thread = std::thread([this, time_limit_s, callback, d_stderr, d_stdout,
                           args = std::make_tuple(args...)]() mutable {
      uint64_t started_at = _timestamp();
      boost::process::child proc_ = std::apply(
          [d_stdout, d_stderr](auto const&... args) {
            return boost::process::child(
                args..., boost::process::std_out > d_stdout, boost::process::std_err > d_stderr);
          },
          args);

      using namespace std::chrono_literals;
      std::unique_lock<std::mutex> lg(_cv_m);

      bool time_limit_exceeded = false;
      while (!_interrupted && proc_.running() && !time_limit_exceeded) {
        _cv.wait_for(lg, 100ms, [this, &proc_]() { return _interrupted || !proc_.running(); });
        time_limit_exceeded = (_timestamp() - started_at) > time_limit_s;
      }

      CHECK(time_limit_exceeded || _interrupted || !proc_.running());
      if (proc_.running()) {
        LOG(INFO) << "Process is still running, killing it.";
        IPS_SYSCALL(kill(proc_.native_handle(), SIGKILL));
        proc_.wait();
      }

      callback(started_at, _timestamp(), proc_.exit_code(), _interrupted, time_limit_exceeded);
    });
  }

  ~Execution() noexcept;

  void interrupt() noexcept;

  void await() noexcept;

 private:
  std::mutex _cv_m;
  std::condition_variable _cv;
  bool _interrupted = false;
  std::thread _thread;
};

}  // namespace infra::execution

#endif  // ITMO_PARSAT_EXECUTION_H
