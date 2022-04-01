#include "core/tests/common/paths.h"

namespace common {

std::vector<std::string> const& inputs(std::string const& group) {
  static std::unordered_map<std::string, std::vector<std::string>> inputs;
  static std::mutex m;
  std::lock_guard<std::mutex> lg(m);
  if (inputs.count(group) == 0) {
    std::set<std::string> inputs_set;
    std::filesystem::path cnf_path(IPS_PROJECT_ROOT "/resources/cnf");
    cnf_path /= group;
    for (auto const& entry : std::filesystem::recursive_directory_iterator(
             cnf_path,
             std::filesystem::directory_options::follow_directory_symlink)) {
      if (entry.is_regular_file() && entry.path().extension() == ".cnf") {
        inputs_set.insert(entry.path().filename());
      }
    }
    inputs[group] = {};
    inputs[group].reserve(inputs_set.size());
    for (auto const& s : inputs_set) {
      inputs[group].push_back(group + "/" + s);
    }
  }
  return inputs[group];
}

}  // namespace common