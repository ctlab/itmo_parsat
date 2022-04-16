#include "core/sat/native/painless/painless-src/working/WorkingStrategy.h"

namespace painless {

WorkingStrategy::WorkingStrategy(WorkingResult* working_result) : result(working_result) {}

void WorkingStrategy::addSlave(WorkingStrategy* slave) {
  slave->parent = this;
  slave->result = result;
  slaves.push_back(slave);
}

std::mutex m_pf;

}  // namespace painless