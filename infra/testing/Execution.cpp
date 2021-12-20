#include "infra/testing/Execution.h"

namespace infra {

Execution::~Execution() noexcept {
  await();
}

void Execution::interrupt() noexcept {
  {
    std::lock_guard<std::mutex> lg(cv_m_);
    interrupted_ = true;
  }
  cv_.notify_all();
}

void Execution::await() noexcept {
  if (thread_.joinable()) {
    thread_.join();
  }
}

}  // namespace infra