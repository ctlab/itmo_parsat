#ifndef ITMO_PARSAT_UTIL_H
#define ITMO_PARSAT_UTIL_H

#include "core/tests/common/get.h"

namespace common {

core::sat::State to_state(Minisat::lbool b);

}  // namespace common

#endif  // ITMO_PARSAT_UTIL_H
