#ifndef ITMO_PARSAT_OPTIONS_H
#define ITMO_PARSAT_OPTIONS_H

#include <execution>

namespace util::opt {

#if defined(IPS_SANITIZE_COMMON) || defined(IPS_SANITIZE_THREAD)

// We disable parallel execution when sanitizing,
// because leak appears to be in tbb (most probably, false positive).
#define IPS_EXEC_POLICY std::execution::seq

#else

#define IPS_EXEC_POLICY std::execution::par_unseq

#endif

std::string get_options_description();

}  // namespace util::opt

#endif  // ITMO_PARSAT_OPTIONS_H
