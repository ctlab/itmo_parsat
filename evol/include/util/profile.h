#ifndef EA_PROFILE_H
#define EA_PROFILE_H

#include <glog/logging.h>

#include <string>

namespace ea::internal_::profile {

void register_start(std::string const& name);

double register_exit(std::string const& name);

std::string make_location(char const* file, char const* func, int line);

}  // namespace ea::internal_::profile

#define MAKE_LOCATION(N, F, FU) N##F##FU

#define LOCATION \
  ea::internal_::profile::make_location(__FILE__, __func__, __LINE__)

#define LOG_TIME(X)                                                          \
  ::ea::internal_::profile::register_start(#X);                              \
  X;                                                                         \
  LOG(INFO) << #X << " took " << ::ea::internal_::profile::register_exit(#X) \
            << "s."

#endif  // EA_PROFILE_H
