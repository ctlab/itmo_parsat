#include "core/util/Tracer.h"

namespace core::trace {

Event::Event(std::string name) : _name(std::move(name)) {}

Event::~Event() noexcept {
  IPS_INFO_T(TRACER, "Registered event: " << _name);

  std::lock_guard<std::mutex> stats_lock(Tracer::instance()._stats_mutex);
  auto& tracer_stats = Tracer::instance()._stats;

  auto it = tracer_stats.find(_name);
  if (it == tracer_stats.end()) {
    tracer_stats[_name] = {1, {}};
  } else {
    auto& stats = it->second;
    ++stats.count;
  }
}

CodeBlock::CodeBlock(std::string name)
    : _name(std::move(name)), _start(std::chrono::system_clock::now()) {}

CodeBlock::~CodeBlock() noexcept {
  std::chrono::duration<float> std_duration = std::chrono::system_clock::now() - _start;
  float duration = std_duration.count();
  IPS_INFO_T(TRACER, _name << " took " << duration << "s");

  std::lock_guard<std::mutex> stats_lock(Tracer::instance()._stats_mutex);
  auto& tracer_stats = Tracer::instance()._stats;

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

Tracer& Tracer::instance() {
  static Tracer _tracer;
  return _tracer;
}

void Tracer::print_summary(size_t num_quantiles) {
  for (auto& p : instance()._stats) {
    std::string const& name = p.first;
    Stats& stats = p.second;

    if (stats.durations.empty()) {
      IPS_INFO("Event " << name << ": total samples: " << stats.count);
    } else {
      std::sort(stats.durations.begin(), stats.durations.end());
      size_t quantiles = std::min(num_quantiles, stats.durations.size());
      size_t step = stats.durations.size() / quantiles;

      std::stringstream info;
      info << "Quantiles for " << name << ", total samples: " << stats.count << ":\n\t[ ";
      for (size_t i = 0; i < stats.durations.size(); i += step) {
        info << stats.durations[i] << ' ';
      }
      info << "]";

      IPS_INFO(info.str());
    }
  }
}

}  // namespace core
