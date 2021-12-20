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
  // clang-format off
  template <typename... Args, typename Stderr, typename Stdout>
  explicit Execution(callback_t const& callback, Stderr d_stderr, Stdout d_stdout, Args&&... args) {
      thread_ = std::thread(
        [this, callback, d_stderr, d_stdout, args = std::make_tuple(std::forward<Args>(args)...)] () mutable {
          uint64_t started_at = _timestamp();
          boost::process::child proc_ = std::apply([d_stdout, d_stderr](auto&&... args) {
            return boost::process::child(
              args..., boost::process::std_out > d_stdout, boost::process::std_err > d_stderr
            );
          }, std::move(args));

          using namespace std::chrono_literals;
          std::unique_lock<std::mutex> lg(cv_m_);

          while (!interrupted_ && proc_.running()) {
            cv_.wait_for(lg, 100ms,
              [this, &proc_]() {
                return interrupted_ || !proc_.running();
              }
            );
          }

          CHECK(interrupted_ || !proc_.running());
          if (proc_.running()) {
            LOG(INFO) << "Process is still running, interrupting it.";
            kill(proc_.native_handle(), SIGINT);
          }

          callback(started_at, _timestamp(), proc_.exit_code(), interrupted_);
      });
  }
  // clang-format on

  ~Execution() noexcept;

  void interrupt() noexcept;

  void await() noexcept;

 private:
  std::mutex cv_m_;
  std::condition_variable cv_;
  bool interrupted_ = false;
  std::thread thread_;
};

}  // namespace infra

#endif  // ITMO_PARSAT_EXECUTION_H
