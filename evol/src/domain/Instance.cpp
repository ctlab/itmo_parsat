#include "evol/include/domain/Instance.h"

namespace ea::instance {

bool operator<(Instance& a, Instance& b) {
  return a.fitness() < b.fitness();
}

}  // namespace ea::instance