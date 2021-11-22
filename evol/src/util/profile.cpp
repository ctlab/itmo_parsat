#include "evol/include/util/profile.h"

#include <chrono>
#include <map>
#include <mutex>
#include <thread>

namespace ea::internal_::profile {

class Timer {
 public:
  void register_start(std::string const& name);

  double register_exit(std::string const& name);

 private:
  std::map<std::string, std::chrono::time_point<std::chrono::system_clock>> start_;
  std::mutex lock_;
};

Timer timer{};

void Timer::register_start(std::string const& name) {
  std::lock_guard<std::mutex> lg(lock_);
  start_[name] = std::chrono::system_clock::now();
}

double Timer::register_exit(std::string const& name) {
  std::lock_guard<std::mutex> lg(lock_);
  std::chrono::duration<double> dur = std::chrono::system_clock::now() - start_[name];
  start_.erase(name);
  return dur.count();
}

void register_start(std::string const& name) {
  timer.register_start(name);
}

double register_exit(std::string const& name) {
  return timer.register_exit(name);
}

}  // namespace ea::internal_::profile
