#ifndef ITMO_PARSAT_TIMETRACER_H
#define ITMO_PARSAT_TIMETRACER_H

#include <string>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <unordered_map>
#include <iomanip>
#include <vector>
#include <sstream>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "Logger.h"
#include "util/Assert.h"

namespace core {

using clock_t = typename std::conditional<
    std::chrono::high_resolution_clock::is_steady,
    std::chrono::high_resolution_clock, std::chrono::steady_clock>::type;

namespace trace {

struct Event {
  explicit Event(std::string name);
  ~Event() noexcept;

 protected:
  std::string _name;
};

struct CodeBlock {
  explicit CodeBlock(std::string name);
  ~CodeBlock() noexcept;

 private:
  std::string _name;
  std::chrono::time_point<clock_t> _start;
};

}  // namespace trace

/**
 * @brief Tracing utility class.
 */
class TimeTracer {
  friend struct trace::Event;
  friend struct trace::CodeBlock;

 private:
  struct EventStats {
    uint64_t count{0};
  };

  struct DurStats {
    uint64_t count{0};
    std::vector<float> durations{};
  };

 private:
  TimeTracer() = default;

  void _clean() noexcept;

  static TimeTracer& instance();

 public:
  static void print_summary(uint32_t num_quantiles);

  static void clean();

 private:
  std::mutex _stats_mutex;
  std::unordered_map<std::string, EventStats> _event_stats;
  std::unordered_map<std::string, DurStats> _dur_stats;
};

}  // namespace core

#ifndef IPS_DISABLE_TRACE

#define IPS_TRACE_SUMMARY(n) ::core::TimeTracer::print_summary(n)

#define IPS_TRACE_N(name, expr)         \
  do {                                  \
    ::core::trace::CodeBlock _cb(name); \
    expr;                               \
  } while (0)

#define IPS_TRACE_N_V(name, expr)                  \
  [&] {                                            \
    ::core::trace::CodeBlock _cb(name);            \
    auto&& result = expr;                          \
    return std::forward<decltype(result)>(result); \
  }()

#define IPS_TRACE(expr) IPS_TRACE_N(#expr, expr)

#define IPS_TRACE_V(expr) IPS_TRACE_N_V(#expr, expr)

#define IPS_EVENT(name) ::core::trace::Event _event(name);

#else

#define IPS_TRACE_SUMMARY(...)

#define IPS_TRACE_N(name, expr) expr

#define IPS_TRACE_N_V(name, expr) expr

#define IPS_TRACE(expr) expr

#define IPS_TRACE_V(expr) expr

#define IPS_EVENT(...)

#endif

#endif  // ITMO_PARSAT_TIMETRACER_H
