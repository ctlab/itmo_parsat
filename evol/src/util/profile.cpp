#include "evol/include/util/profile.h"

#include <thread>

namespace ea::internal_::profile {

Timer timer{};

void Timer::register_start(std::string const& name) {
  std::lock_guard<std::mutex> lg(lock_);
  start_[name] = std::chrono::system_clock::now();
}

double Timer::register_exit(std::string const& name) {
  std::lock_guard<std::mutex> lg(lock_);
  std::chrono::duration<double> dur
    = std::chrono::system_clock::now() - start_[name];
  return dur.count();
}

std::string make_location(char const* file, char const* func, int line) {
  return std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()))
    + std::string(file) + std::string(func) + std::to_string(line);
}

} // namespace ea::internal_::profile
