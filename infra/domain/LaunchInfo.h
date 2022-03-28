#ifndef ITMO_PARSAT_LAUNCHINFO_H
#define ITMO_PARSAT_LAUNCHINFO_H

#include "infra/domain/LaunchesDao.h"
#include "infra/fixture/TestingConfiguration.h"
#include "infra/domain/LaunchConfig.h"
#include "util/CliConfig.h"

namespace infra::domain {

class LaunchInfo {
 public:
  explicit LaunchInfo(fixture::TestingConfiguration testing_config);

  bool should_be_launched(LaunchConfig const& config);

  std::optional<uint32_t> check_if_test_is_done(LaunchConfig const& config);

  static int get_test_size(LaunchConfig const& config);

  static std::string get_test_group(LaunchConfig const& config);

  static std::string get_input_name(LaunchConfig const& config);

  static infra::domain::SatResult get_sat_result(LaunchConfig const& config);

  static uint32_t get_threads_required(std::filesystem::path const& solve_config);

  void add(LaunchObject const& object);

 private:
  fixture::TestingConfiguration _t_config;
  std::optional<LaunchesDao> _dao;
};

}  // namespace infra::domain

#endif  // ITMO_PARSAT_LAUNCHINFO_H
