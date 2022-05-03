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

namespace util {

using clock_t = typename std::conditional<
    std::chrono::high_resolution_clock::is_steady, std::chrono::high_resolution_clock, std::chrono::steady_clock>::type;

namespace _detail {

struct Event {
  explicit Event(std::atomic<int64_t>* count);
};

struct CodeBlock {
  CodeBlock(std::atomic<int64_t>* count, std::atomic<int64_t>* time);

  ~CodeBlock() noexcept;

 private:
  clock_t::time_point _start;
  std::atomic<int64_t>* _count;
  std::atomic<int64_t>* _time;
};

}  // namespace _detail

/**
 * @brief Almost zero-cost time tracing methods.
 */
class TimeTracer {
 public:
  typedef std::atomic<int64_t> event_t;

  struct cblock_t {
    std::atomic<int64_t> count{};
    std::atomic<int64_t> time{};
  };

  struct Events {
    cblock_t algorithm_step{};
    cblock_t algorithm_preprocess{};
    cblock_t algorithm_prepare{};
    cblock_t algorithm_process{};
    cblock_t cartesian_search{};
    cblock_t recurr_filter_fast{};
    cblock_t reduce_solve_tasks{};
    cblock_t problem_eliminate{};
    cblock_t problem_construct{};
    cblock_t solve_solve{};
    cblock_t solver_propagate{};
    cblock_t solver_solve{};
    cblock_t solver_load_problem{};
    cblock_t filter_conflict{};
    cblock_t prop_load_problem{};
    cblock_t solver_service_load_problem{};
    cblock_t solver_service_solve_undef{};
    cblock_t solver_service_solve_time_limit{};

    event_t algorithm_cache_hit{};
  } events{};

  static void print_summary();

  static void clean();

  static TimeTracer& instance();

 private:
  TimeTracer() = default;

  void _clean() noexcept;
};

}  // namespace util

#ifndef IPS_DISABLE_TRACE

#define IPS_TRACE_SUMMARY ::util::TimeTracer::print_summary()

#define IPS_EVENT(TYPE) ::util::_detail::Event(&::util::TimeTracer::instance().events.TYPE)

#define IPS_BLOCK(TYPE, EXPR)                                                            \
  do {                                                                                   \
    auto& tracer = ::util::TimeTracer::instance();                                       \
    ::util::_detail::CodeBlock _cb(&tracer.events.TYPE.count, &tracer.events.TYPE.time); \
    EXPR;                                                                                \
  } while (0)

#define IPS_BLOCK_R(TYPE, EXPR)                                                          \
  [&] {                                                                                  \
    auto& tracer = ::util::TimeTracer::instance();                                       \
    ::util::_detail::CodeBlock _cb(&tracer.events.TYPE.count, &tracer.events.TYPE.time); \
    auto&& result = EXPR;                                                                \
    return std::forward<decltype(result)>(result);                                       \
  }()

#else

#define IPS_TRACE_SUMMARY

#define IPS_EVENT(TYPE)

#define IPS_BLOCK(TYPE, EXPR) EXPR

#define IPS_BLOCK_R(TYPE, EXPR) EXPR

#endif

#endif  // ITMO_PARSAT_TIMETRACER_H
