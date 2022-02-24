#ifndef ITMO_PARSAT_EXECUTIONMANAGER_H
#define ITMO_PARSAT_EXECUTIONMANAGER_H

#include <boost/process.hpp>

#include "infra/execution/Execution.h"
#include "util/Semaphore.h"

namespace infra::execution {

class ExecutionManager {
 public:
  explicit ExecutionManager(uint32_t cpu_limit);

  template <typename... Args, typename Stderr, typename Stdout>
  void execute(
      uint32_t cpu_limit, Execution::callback_t const& callback, Stderr d_stderr, Stdout d_stdout,
      uint64_t time_limit_s, Args const&... args) {
    IPS_INFO("EXECUTE");
    // clang-format off
    _semaphore.acquire(cpu_limit);
    _execs.push_back(std::make_shared<Execution>(
      [callback, this, cpu_limit](
        uint64_t started_at, uint64_t finished_at, int exit, bool interrupt, bool tle
      ) {
        _semaphore.release(cpu_limit);
        callback(started_at, finished_at, exit, interrupt, tle);
    }, d_stderr, d_stdout, time_limit_s, args...));
    // clang-format on
  }

  void interrupt();

  void await();

 private:
  std::vector<std::shared_ptr<Execution>> _execs;
  util::Semaphore _semaphore;
};

}  // namespace infra::execution

#endif  // ITMO_PARSAT_EXECUTIONMANAGER_H
