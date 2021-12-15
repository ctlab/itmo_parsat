#include "core/util/Tracer.h"

namespace {

core::Tracer* _tracer = nullptr;

}  // namespace

namespace core {

Tracer::Tracer() {
  _tracer = this;
}

Tracer::~Tracer() noexcept {
  _tracer = nullptr;
}

void Tracer::start_trace(const std::string& identifier) {
  if (_tracer != nullptr) {
    _tracer->_start_trace(identifier);
  }
}

void Tracer::end_trace(const std::string& identifier) {
  if (_tracer != nullptr) {
    return _tracer->_end_trace(identifier);
  }
}

void Tracer::_start_trace(const std::string& identifier) {
  std::string const& full_id =
      identifier + std::to_string(std::hash<std::thread::id>()(std::this_thread::get_id()));
  std::lock_guard<std::mutex> lg(m_);
  CHECK(start_.find(full_id) == start_.end()) << "Double call of start_trace";
  start_[full_id] = std::chrono::system_clock::now();
}

void Tracer::_end_trace(const std::string& identifier) {
  std::string const& full_id =
      identifier + std::to_string(std::hash<std::thread::id>()(std::this_thread::get_id()));
  std::lock_guard<std::mutex> lg(m_);
  auto it = start_.find(full_id);
  CHECK(it != start_.end()) << "end_trace with no start_trace";
  std::chrono::duration<double> dur = std::chrono::system_clock::now() - it->second;
  start_.erase(it);
  LOG(INFO) << identifier << " took " << dur.count() << "s";
}

}  // namespace core