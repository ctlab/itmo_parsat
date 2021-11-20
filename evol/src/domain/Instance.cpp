#include "evol/include/domain/Instance.h"

namespace ea::instance {

bool operator<(Instance const& a, Instance const& b) {
  return a.fitness() < b.fitness();
}

}  // namespace ea::instance