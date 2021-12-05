#ifndef EA_PROFILE_H
#define EA_PROFILE_H

#include "evol/include/util/Logging.h"

#include <string>

namespace ea::internal_::profile {

void register_start(std::string const& name);

double register_exit(std::string const& name);

}  // namespace ea::internal_::profile

#define LOG_TIME(X)                             \
  ::ea::internal_::profile::register_start(#X); \
  X;                                            \
  EALOG(LogType::TIME) << #X << " took " << ::ea::internal_::profile::register_exit(#X) << "s."

#endif  // EA_PROFILE_H
