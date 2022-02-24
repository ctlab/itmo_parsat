#ifndef ITMO_PARSAT_LAUNCHINFO_H
#define ITMO_PARSAT_LAUNCHINFO_H

#include "infra/domain/LaunchesDao.h"
#include "infra/testing/TestingConfiguration.h"
#include "infra/domain/LaunchConfig.h"

namespace infra::domain {

class LaunchInfo {
 public:
  explicit LaunchInfo(testing::TestingConfiguration const& testing_config);

  bool should_be_launched(LaunchConfig const& config);

  std::optional<uint32_t> check_if_test_is_done(LaunchConfig const& config);

  int get_test_size(LaunchConfig const& config);

  std::string get_test_group(LaunchConfig const& config);

  infra::domain::SatResult get_sat_result(LaunchConfig const& config);

  void add(LaunchObject const& object);

 private:
  testing::TestingConfiguration _t_config;
  std::optional<LaunchesDao> _dao;
};

}  // namespace infra::domain

#endif  // ITMO_PARSAT_LAUNCHINFO_H
