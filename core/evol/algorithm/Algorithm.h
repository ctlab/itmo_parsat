#ifndef EA_BASE_ALGORITHM_H
#define EA_BASE_ALGORITHM_H

#include <utility>
#include <thread>

#include "core/proto/solve_config.pb.h"
#include "core/util/Registry.h"
#include "core/evol/instance/Instance.h"
#include "core/evol/instance/SharedData.h"
#include "core/evol/limit/Limit.h"
#include "core/evol/method/select/Selector.h"
#include "core/util/Logger.h"
#include "core/util/Tracer.h"

namespace ea::algorithm {

class Algorithm {
 private:
  void _init_shared_data(InstanceConfig const& config);

 protected:
  void _add_instance();

  virtual void _prepare();

 public:
  virtual ~Algorithm() = default;

  void prepare();

  explicit Algorithm(BaseAlgorithmConfig const& config);

  void process();

  void interrupt();

  core::sat::Solver& get_solver() noexcept;

  instance::Population& get_population() noexcept;

  instance::Instance& get_best() noexcept;

  instance::SharedData& get_shared_data() noexcept;

  [[nodiscard]] size_t inaccurate_points() const noexcept;

 protected:
  virtual void step() = 0;

  [[nodiscard]] bool is_interrupted() const;

 protected:
  InstanceConfig _instance_config;
  instance::Population _population;
  instance::RSharedData _shared_data;
  core::sat::RSolver _solver;

 private:
  limit::RLimit _limit;
  bool _interrupted = false;
};

using RAlgorithm = std::shared_ptr<Algorithm>;

DEFINE_REGISTRY(Algorithm, AlgorithmConfig, algorithm);

}  // namespace ea::algorithm

#endif  // EA_BASE_ALGORITHM_H
