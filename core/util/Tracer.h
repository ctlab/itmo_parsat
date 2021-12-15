#ifndef ITMO_PARSAT_TRACER_H
#define ITMO_PARSAT_TRACER_H

#include <string>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <glog/logging.h>

namespace core {

class Tracer {
 public:
  Tracer();

  ~Tracer() noexcept;

  static void start_trace(std::string const& identifier);

  static void end_trace(std::string const& identifier);

 private:
  void _start_trace(std::string const& identifier);

  void _end_trace(std::string const& identifier);

 private:
  std::mutex m_;
  std::unordered_map<std::string, std::chrono::system_clock::time_point> start_;
};

#define IPS_TRACE_NAMED(name, expr)  \
  ::core::Tracer::start_trace(name); \
  expr;                              \
  ::core::Tracer::end_trace(name)

#define IPS_TRACE(expr) IPS_TRACE_NAMED(#expr, expr)

}  // namespace core

#endif  // ITMO_PARSAT_TRACER_H
