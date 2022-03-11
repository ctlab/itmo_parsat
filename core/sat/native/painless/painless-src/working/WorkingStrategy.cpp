#include "core/sat/native/painless/painless-src/working/WorkingStrategy.h"

namespace painless {

WorkingStrategy::WorkingStrategy(WorkingResult* working_result) : result(working_result) {}

void WorkingStrategy::addSlave(WorkingStrategy* slave) {
  slaves.push_back(slave);
  slave->parent = this;
  slave->result = result;
}

}  // namespace painless