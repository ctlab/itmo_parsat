#include "util/options.h"

#include <sstream>

namespace {

#if defined(IPS_SANITIZE_COMMON)

char const* sanitize = "common";

#elif defined(IPS_SANITIZE_THREAD)

char const* sanitize = "thread";

#else

char const* sanitize = "disabled";

#endif

#if defined(IPS_DISABLE_LOG)

char const* log = "disabled";

#else

char const* log = "enabled";

#endif

#if defined(IPS_DISABLE_TRACE)

char const* trace = "disabled";

#else

char const* trace = "enabled";

#endif

#if defined(IPS_DISABLE_VERIFY)

char const* verify = "disabled";

#else

char const* verify = "enabled";

#endif

#define STR(x) #x

#define STRINGIFY(x) STR(x)

#define IPS_GIT_HASH STRINGIFY(GIT_HASH)

#define IPS_BUILD_MODE STRINGIFY(CMAKE_BUILD_TYPE)

}  // namespace

namespace util::opt {

std::string get_options_description() {
  std::stringstream ss;
  ss << "itmo-parsat: parallel SAT solver library\n"
     << "\tGit hash:\t\t" << IPS_GIT_HASH << "\n"
     << "\tBuild type:\t\t" << IPS_BUILD_MODE << "\n"
     << "\tSanitizers mode:\t" << sanitize << "\n"
     << "\tLogging mode:\t\t" << log << "\n"
     << "\tTracing mode:\t\t" << trace << "\n"
     << "\tVerification mode:\t" << verify;
  return ss.str();
}

}  // namespace util::opt
