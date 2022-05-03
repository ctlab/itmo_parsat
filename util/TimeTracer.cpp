#include "TimeTracer.h"

namespace util::_detail {

Event::Event(std::atomic<int64_t>* count) { count->fetch_add(1, std::memory_order_relaxed); }

CodeBlock::CodeBlock(std::atomic<int64_t>* count, std::atomic<int64_t>* time)
    : _start(clock_t::now()), _count(count), _time(time) {}

CodeBlock::~CodeBlock() noexcept {
  using namespace std::chrono;
  int64_t dur = duration_cast<microseconds>(clock_t::now() - _start).count();
  _count->fetch_add(1, std::memory_order_relaxed);
  _time->fetch_add(dur, std::memory_order_relaxed);
}

}  // namespace util::_detail

namespace util {

TimeTracer& TimeTracer::instance() {
  static TimeTracer _tracer;
  return _tracer;
}

void TimeTracer::print_summary() {
  std::stringstream info, quant;
  info << std::fixed;
  quant << std::fixed;
  info << std::setprecision(4);
  quant << std::setprecision(4);

  auto const& events = instance().events;
  info << "Tracing summary:";
  info << "\n"
       << std::setw(32) << "Name" << std::setw(10) << "Count" << std::setw(10)  //
       << "Total (s)" << std::setw(10) << "Avg (s)" << std::setw(12) << "Bandwidth";

#define PRINT_BLOCK(TYPE)                                                                                       \
  do {                                                                                                          \
    int64_t count = events.TYPE.count.load(std::memory_order_relaxed);                                          \
    if (count == 0) {                                                                                           \
      break;                                                                                                    \
    }                                                                                                           \
    uint64_t time_ms = events.TYPE.time.load(std::memory_order_relaxed);                                        \
    float time_s = static_cast<float>(time_ms) / 1000000.f;                                                     \
    float bandwidth = static_cast<float>(count) / time_s;                                                       \
    float avg = time_s / count;                                                                                 \
    info << "\n"                                                                                                \
         << std::setw(32) << #TYPE << std::setw(10) << count << std::setw(10) << time_s << std::setw(10) << avg \
         << std::setw(12) << (bandwidth > 100000 ? -1 : bandwidth);                                             \
  } while (false)

#define PRINT_EVENT(TYPE)                                                                                      \
  do {                                                                                                         \
    int64_t count = events.TYPE.load(std::memory_order_relaxed);                                               \
    info << "\n"                                                                                               \
         << std::setw(32) << #TYPE << std::setw(10) << count << std::setw(10) << "--" << std::setw(10) << "--" \
         << std::setw(10) << "--";                                                                             \
  } while (false)

  PRINT_BLOCK(algorithm_step);
  PRINT_BLOCK(algorithm_preprocess);
  PRINT_BLOCK(algorithm_prepare);
  PRINT_BLOCK(algorithm_process);
  PRINT_BLOCK(cartesian_search);
  PRINT_BLOCK(recurr_filter_fast);
  PRINT_BLOCK(reduce_solve_tasks);
  PRINT_BLOCK(problem_eliminate);
  PRINT_BLOCK(problem_construct);
  PRINT_BLOCK(solve_solve);
  PRINT_BLOCK(solver_propagate);
  PRINT_BLOCK(solver_solve);
  PRINT_BLOCK(solver_load_problem);
  PRINT_BLOCK(filter_conflict);
  PRINT_BLOCK(prop_load_problem);
  PRINT_BLOCK(solver_service_load_problem);
  PRINT_BLOCK(solver_service_solve_undef);
  PRINT_BLOCK(solver_service_solve_time_limit);
  PRINT_EVENT(algorithm_cache_hit);

  IPS_INFO(info.str() << quant.str());
}  // namespace util

void TimeTracer::_clean() noexcept {
#define CLEAR_BLOCK(TYPE)                                \
  events.TYPE.count.store(0, std::memory_order_relaxed); \
  events.TYPE.time.store(0, std::memory_order_relaxed)
#define CLEAR_EVENT(TYPE) events.TYPE.store(0, std::memory_order_relaxed)
  CLEAR_BLOCK(algorithm_step);
  CLEAR_BLOCK(algorithm_preprocess);
  CLEAR_BLOCK(algorithm_prepare);
  CLEAR_BLOCK(algorithm_process);
  CLEAR_BLOCK(cartesian_search);
  CLEAR_BLOCK(recurr_filter_fast);
  CLEAR_BLOCK(reduce_solve_tasks);
  CLEAR_BLOCK(problem_eliminate);
  CLEAR_BLOCK(problem_construct);
  CLEAR_BLOCK(solve_solve);
  CLEAR_BLOCK(solver_propagate);
  CLEAR_BLOCK(solver_solve);
  CLEAR_BLOCK(solver_load_problem);
  CLEAR_BLOCK(filter_conflict);
  CLEAR_BLOCK(prop_load_problem);
  CLEAR_BLOCK(solver_service_load_problem);
  CLEAR_BLOCK(solver_service_solve_undef);
  CLEAR_BLOCK(solver_service_solve_time_limit);
  CLEAR_EVENT(algorithm_cache_hit);
}

void TimeTracer::clean() { instance()._clean(); }

}  // namespace util
