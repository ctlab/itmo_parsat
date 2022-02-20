#ifndef ITMO_PARSAT_OPTIONS_H
#define ITMO_PARSAT_OPTIONS_H

#include <execution>

namespace util::opt {

#ifdef IPS_SANITIZE

// We disable parallel execution when sanitizing,
// because leak appears to be in tbb (most probably, false positive).
#define IPS_EXEC_POLICY std::execution::seq

#else

#define IPS_EXEC_POLICY std::execution::par_unseq

#endif

}  // namespace util::opt

#endif  // ITMO_PARSAT_OPTIONS_H
