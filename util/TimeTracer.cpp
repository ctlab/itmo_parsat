#include "TimeTracer.h"

namespace core::trace {

Event::Event(std::string name) : _name(std::move(name)) {}

Event::~Event() noexcept {
  IPS_INFO_T(TRACER, "Registered event: " << _name);
  std::lock_guard<std::mutex> stats_lock(TimeTracer::instance()._stats_mutex);
  auto& tracer_stats = TimeTracer::instance()._event_stats;
  auto it = tracer_stats.find(_name);
  if (it == tracer_stats.end()) {
    tracer_stats[_name] = {1};
  } else {
    ++it->second.count;
  }
}

CodeBlock::CodeBlock(std::string name)
    : _name(std::move(name)), _start(clock_t::now()) {}

CodeBlock::~CodeBlock() noexcept {
  std::chrono::duration<float> std_duration = clock_t::now() - _start;
  float duration = std_duration.count();
  IPS_INFO_T(TRACER, _name << " took " << duration << "s");
  std::lock_guard<std::mutex> stats_lock(TimeTracer::instance()._stats_mutex);
  auto& tracer_stats = TimeTracer::instance()._dur_stats;
  auto it = tracer_stats.find(_name);
  if (it == tracer_stats.end()) {
    tracer_stats[_name] = {1, {duration}};
  } else {
    auto& stats = it->second;
    ++stats.count;
    stats.durations.push_back(duration);
  }
}

}  // namespace core::trace

namespace core {

TimeTracer& TimeTracer::instance() {
  static TimeTracer _tracer;
  return _tracer;
}

void TimeTracer::print_summary(uint32_t num_quantiles) {
  std::stringstream info, quant;
  info << std::fixed;
  quant << std::fixed;
  info << std::setprecision(2);
  quant << std::setprecision(2);

  info << "Tracing summary:";
  info << "\n" << std::setw(32) << "Name" << std::setw(10) << "Count";
  quant << "\n" << std::setw(32) << "Name" << std::setw(10) << "Quantiles";

  for (auto& p : instance()._event_stats) {
    std::string const& name = p.first;
    EventStats const& stats = p.second;
    info << "\n" << std::setw(32) << name << std::setw(10) << stats.count;
  }
  info << "\n"
       << std::setw(32) << "Name" << std::setw(12) << "Count" << std::setw(12)
       << "Total(s)" << std::setw(12) << "Min(s)" << std::setw(12) << "Max(s)";

  for (auto& p : instance()._dur_stats) {
    std::string const& name = p.first;
    DurStats& stats = p.second;
    float total_time = 0.f;
    float min_time = 0.f;
    float max_time = 0.f;
    std::for_each(stats.durations.begin(), stats.durations.end(), [&](float t) {
      total_time += t;
      min_time = std::min(min_time, t);
      max_time = std::max(max_time, t);
    });
    size_t quantiles =
        std::min(num_quantiles, (uint32_t) stats.durations.size());
    size_t step = stats.durations.size() / quantiles;
    info << "\n"
         << std::setw(32) << name << std::setw(12) << stats.count
         << std::setw(12) << total_time << std::setw(12) << min_time
         << std::setw(12) << max_time;
    quant << "\n" << std::setw(32) << name << " [ ";
    for (size_t i = 0; i < stats.durations.size(); i += step) {
      std::nth_element(
          stats.durations.begin(), stats.durations.begin() + i,
          stats.durations.end());
      quant << stats.durations[i] << ' ';
    }
    quant << "]";
  }
  IPS_INFO(info.str() << quant.str());
}

void TimeTracer::_clean() noexcept {
  std::lock_guard<std::mutex> lg(_stats_mutex);
  _event_stats.clear();
  _dur_stats.clear();
}

void TimeTracer::clean() {
  instance()._clean();
}

}  // namespace core
