#ifndef EA_BASE_ALGORITHM_H
#define EA_BASE_ALGORITHM_H

#include <utility>

#include "evol/include/domain/Instance.h"
#include "evol/include/limit/Limit.h"
#include "evol/include/method/select/Selector.h"
#include "evol/include/util/profile.h"
#include "evol/proto/config.pb.h"

namespace ea::algorithm {

class Algorithm {
 private:
  void _init_shared_data(AlgorithmConfig_InstanceConfig const& config);

 protected:
  void _add_instance();

  virtual void _prepare();

 public:
  virtual ~Algorithm() = default;

  Algorithm();

  void prepare();

  explicit Algorithm(AlgorithmConfig const& config);

  void process();

  void interrupt();

  sat::Solver& get_solver() noexcept;

  domain::Population& get_population() noexcept;

  domain::Instance& get_best() noexcept;

  [[nodiscard]] size_t inaccurate_points() const noexcept;

 protected:
  virtual void step() = 0;

  [[nodiscard]] bool is_interrupted() const;

 protected:
  AlgorithmConfig_InstanceConfig instance_config_;
  domain::Population population_;
  domain::Instance::RSharedData shared_data_;
  sat::RSolver solver_;

 private:
  limit::RLimit limit_;
  bool interrupted_ = false;
};

using RAlgorithm = std::shared_ptr<Algorithm>;

}  // namespace ea::algorithm

#endif  // EA_BASE_ALGORITHM_H
