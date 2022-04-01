#include "core/tests/common/paths.h"

namespace common {

core::sat::Problem get_problem(std::string const& input_name, bool eliminate) {
  static std::filesystem::path path = IPS_PROJECT_ROOT "/resources/cnf";
  return core::sat::Problem(path / input_name, eliminate);
}

std::vector<core::sat::Problem> const& problems(
    bool eliminate, bool allow_trivial, std::string const& group) {
  static std::unordered_map<std::string, std::vector<core::sat::Problem>>
      inputs;
  static std::mutex m;
  std::lock_guard<std::mutex> lg(m);
  std::string key =
      std::to_string(eliminate) + std::to_string(allow_trivial) + group;
  if (inputs.count(key) == 0) {
    std::set<core::sat::Problem> inputs_set;
    std::filesystem::path cnf_path(IPS_PROJECT_ROOT "/resources/cnf");
    cnf_path /= group;

    std::vector<std::future<core::sat::Problem>> f_problems;
    for (auto const& entry : std::filesystem::recursive_directory_iterator(
             cnf_path,
             std::filesystem::directory_options::follow_directory_symlink)) {
      if (entry.is_regular_file() && entry.path().extension() == ".cnf") {
        f_problems.push_back(std::async(
            std::launch::async,
            [group, eliminate, name = entry.path().filename().string()] {
              return common::get_problem(group + "/" + name, eliminate);
            }));
      }
    }
    for (auto& f_problem : f_problems) {
      auto problem = f_problem.get();
      if (!allow_trivial) {
        if (problem.get_result() != core::sat::UNKNOWN) {
          continue;
        }
      }
      inputs_set.insert(problem);
    }

    inputs[key] = {};
    inputs[key].reserve(inputs_set.size());
    for (auto const& problem : inputs_set) {
      inputs[key].push_back(problem);
    }
  }
  return inputs[key];
}

}  // namespace common