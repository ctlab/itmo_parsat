#include "infra/execution/ExecutionManager.h"

namespace infra::execution {

ExecutionManager::ExecutionManager(uint32_t cpu_limit) {
  _semaphore.set_max(cpu_limit);
}

void ExecutionManager::interrupt() {
  for (auto& exec : _execs) {
    exec->interrupt();
  }
}

void ExecutionManager::await() {
  for (auto& exec : _execs) {
    exec->await();
  }
  _execs.clear();
}

}  // namespace infra::execution