#ifndef ITMO_PARSAT_TRACER_H
#define ITMO_PARSAT_TRACER_H

#include <string>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "core/util/Logger.h"
#include "core/util/assert.h"

namespace core {

class Tracer {
 private:
  Tracer() = default;

 public:
  /**
   * Starts tracing of `identifier'.
   * Only one concurrent trace if `identifier' can occurr.
   */
  static void start_trace(std::string const& identifier);

  /**
   * Ends tracing of `identifier' and logs execution time.
   */
  static void end_trace(std::string const& identifier);

  /**
   * Tracer can only be accessed through this method (maybe indirectly).
   */
  static Tracer& instance();

 private:
  void _start_trace(std::string const& identifier);

  void _end_trace(std::string const& identifier);

 private:
  std::mutex m_;
  std::unordered_map<std::string, std::chrono::system_clock::time_point> start_;
};

}  // namespace core

#ifndef DISABLE_TRACE

#define IPS_TRACE_NAMED(name, expr)  \
  ::core::Tracer::start_trace(name); \
  expr;                              \
  ::core::Tracer::end_trace(name)

#define IPS_TRACE_V_NAMED(name, expr)              \
  [&] {                                            \
    ::core::Tracer::start_trace(name);             \
    auto&& result = expr;                          \
    ::core::Tracer::end_trace(name);               \
    return std::forward<decltype(result)>(result); \
  }()

#else

#define IPS_TRACE_NAMED(name, expr) expr
#define IPS_TRACE_V_NAMED(name, expr) expr

#endif

#define IPS_TRACE(expr) IPS_TRACE_NAMED(#expr, expr)

#define IPS_TRACE_V(expr) IPS_TRACE_V_NAMED(#expr, expr)

#endif  // ITMO_PARSAT_TRACER_H
