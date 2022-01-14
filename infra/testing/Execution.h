#ifndef ITMO_PARSAT_EXECUTION_H
#define ITMO_PARSAT_EXECUTION_H

#include <cstdint>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <boost/process.hpp>
#include <filesystem>
#include <glog/logging.h>

#include "core/util/Tracer.h"

namespace infra {

class Execution {
 private:
  static uint64_t _timestamp() noexcept {
    return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
  }

 public:
  using callback_t = std::function<void(uint64_t, uint64_t, int, bool)>;

 public:
  template <typename... Args, typename Stderr, typename Stdout>
  explicit Execution(callback_t const& callback, Stderr d_stderr, Stdout d_stdout, Args&&... args) {
    _thread = std::thread([this, callback, d_stderr, d_stdout,
                           args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
      uint64_t started_at = _timestamp();
      boost::process::child proc_ = std::apply(
          [d_stdout, d_stderr](auto&&... args) {
            return boost::process::child(
                args..., boost::process::std_out > d_stdout, boost::process::std_err > d_stderr);
          },
          std::move(args));

      using namespace std::chrono_literals;
      std::unique_lock<std::mutex> lg(_cv_m);

      while (!_interrupted && proc_.running()) {
        _cv.wait_for(lg, 100ms, [this, &proc_]() { return _interrupted || !proc_.running(); });
      }

      CHECK(_interrupted || !proc_.running());
      if (proc_.running()) {
        LOG(INFO) << "Process is still running, killing it.";
        IPS_SYSCALL(kill(proc_.native_handle(), SIGKILL));
        proc_.wait();
      }

      callback(started_at, _timestamp(), proc_.exit_code(), _interrupted);
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

}  // namespace infra

#endif  // ITMO_PARSAT_EXECUTION_H
