#ifndef ITMO_PARSAT_TRACER_H
#define ITMO_PARSAT_TRACER_H

#include <string>
#include <numeric>
#include <chrono>
#include <unordered_map>
#include <iomanip>
#include <vector>
#include <sstream>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "core/util/Logger.h"
#include "core/util/assert.h"

namespace core {

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
  std::chrono::time_point<std::chrono::system_clock> _start;
};

}  // namespace trace

class Tracer {
  friend struct trace::Event;
  friend struct trace::CodeBlock;

 private:
  struct Stats {
    uint32_t count{0};
    std::vector<float> durations{};
  };

 private:
  Tracer() = default;

  static Tracer& instance();

 public:
  static void print_summary(size_t num_quantiles);

 private:
  std::mutex _stats_mutex;
  std::unordered_map<std::string, Stats> _stats;
};

}  // namespace core

#ifndef DISABLE_TRACE

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

#define IPS_TRACE_N(name, expr) expr

#define IPS_TRACE_N_V(name, expr) expr

#define IPS_TRACE(expr) expr

#define IPS_TRACE_V(expr) expr

#define IPS_EVENT(...)

#endif

#endif  // ITMO_PARSAT_TRACER_H
