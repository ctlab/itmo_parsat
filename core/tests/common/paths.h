#ifndef ITMO_PARSAT_PATHS_H
#define ITMO_PARSAT_PATHS_H

#include <string>
#include <vector>
#include <filesystem>
#include <mutex>
#include <future>
#include <unordered_map>

#include "util/TimeTracer.h"
#include "core/sat/Problem.h"

namespace common {

static const std::string configs_path =
    IPS_PROJECT_ROOT "/core/tests/resources/";
static const std::string preprocess_config_path =
    configs_path + "preprocess.json";
static const std::string log_config_path = configs_path + "log.json";

core::sat::Problem get_problem(std::string const& input_name, bool eliminate);

std::vector<core::sat::Problem> const& problems(
    bool eliminate, bool allow_trivial, std::string const& group);

template <typename... Args>
std::vector<core::sat::Problem> problems(
    bool eliminate, bool allow_trivial, std::string const& group,
    Args... args) {
  auto tail = problems(eliminate, allow_trivial, args...);
  auto cur = problems(eliminate, allow_trivial, group);
  cur.insert(cur.end(), tail.begin(), tail.end());
  return cur;
}

}  // namespace common

#endif  // ITMO_PARSAT_PATHS_H
