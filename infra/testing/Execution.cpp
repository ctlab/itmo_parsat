#include "infra/testing/Execution.h"

namespace infra {

Execution::~Execution() noexcept {
  await();
}

void Execution::interrupt() noexcept {
  {
    std::lock_guard<std::mutex> lg(_cv_m);
    _interrupted = true;
  }
  _cv.notify_all();
}

void Execution::await() noexcept {
  if (_thread.joinable()) {
    _thread.join();
  }
}

}  // namespace infra