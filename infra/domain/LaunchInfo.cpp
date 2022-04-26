#include "infra/domain/LaunchInfo.h"
#include "core/proto/solve_config.pb.h"

#include <utility>

namespace infra::domain {

LaunchInfo::LaunchInfo(fixture::TestingConfiguration testing_config) : _t_config(std::move(testing_config)) {
  if (_t_config.save || _t_config.lookup) {
    _dao.emplace(_t_config.pg_host);
  }
}

std::string LaunchInfo::get_test_group(LaunchConfig const& config) {
  return std::filesystem::path(config.input).parent_path().filename();
}

std::string LaunchInfo::get_input_name(LaunchConfig const& config) {
  return std::filesystem::path(config.input).filename();
}

bool LaunchInfo::should_be_launched(LaunchConfig const& config) {
  if (std::find(_t_config.test_groups.begin(), _t_config.test_groups.end(), get_test_group(config)) ==
      _t_config.test_groups.end()) {
    return false;
  }
  if (get_sat_result(config) != infra::domain::UNSAT && _t_config.unsat_only) {
    return false;
  }
  int test_size = get_test_size(config);
  if (test_size == -1 && !_t_config.allow_unspecified_size) {
    return false;
  }
  if (test_size > _t_config.size) {
    return false;
  }
  if (_t_config.lookup) {
    auto opt_launch_id = check_if_test_is_done(config);
    if (opt_launch_id.has_value()) {
      IPS_INFO("Test is already done. LaunchId = " << opt_launch_id.value());
      return false;
    }
  }
  return true;
}

int LaunchInfo::get_test_size(LaunchConfig const& config) {
  auto const& name = config.input;
  auto it = name.find('@');
  if (it == std::string::npos) {
    return -1;
  } else {
    return name[it + 1] - '0';
  }
}

infra::domain::SatResult LaunchInfo::get_sat_result(LaunchConfig const& config) {
  auto filename = std::filesystem::path(config.input).filename().string();
  if (filename.find("unsat", 0) != std::string::npos) {
    return infra::domain::UNSAT;
  } else if (filename.find("sat", 0) != std::string::npos) {
    return infra::domain::SAT;
  } else {
    return infra::domain::UNKNOWN;
  }
}

std::optional<uint32_t> LaunchInfo::check_if_test_is_done(LaunchConfig const& config) {
  LaunchObject launch;
  launch.test_group = get_test_group(config);
  launch.input_name = get_input_name(config);
  launch.config_name = config.config;
  launch.branch = _t_config.branch;
  launch.commit_hash = _t_config.commit;
  return _dao->get_launch_id(launch);
}

uint32_t LaunchInfo::get_threads_required(std::filesystem::path const& solve_config) {
  SolveConfig slv_cfg;
  util::CliConfig::read_config(solve_config, slv_cfg);
  return slv_cfg.cpu_limit();
}

void LaunchInfo::add(const LaunchObject& object) {
  // We do not add interrupted launches to DB.
  if (_t_config.save && object.result != INTERRUPTED) {
    _dao->add(object);
  }
}

}  // namespace infra::domain