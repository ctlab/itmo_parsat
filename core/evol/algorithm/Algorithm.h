#ifndef EA_BASE_ALGORITHM_H
#define EA_BASE_ALGORITHM_H

#include <utility>
#include <thread>

#include "core/sat/prop/Prop.h"
#include "core/proto/solve_config.pb.h"
#include "core/domain/SearchSpace.h"
#include "util/Registry.h"
#include "core/evol/instance/Instance.h"
#include "core/evol/instance/SharedData.h"
#include "core/evol/limit/Limit.h"
#include "core/evol/method/select/Selector.h"
#include "core/evol/method/preprocess/Preprocess.h"
#include "util/Logger.h"
#include "util/TimeTracer.h"

namespace ea::algorithm {

/**
 * @brief Statistics of rho-backdoor search.
 * @note Statistics collection must be enabled in the configuration.
 * @note Statistics collection results in performance degradation.
 */
struct AlgorithmStatistics {
  int64_t iterations = 0;
  int64_t duration_ms = 0;
  std::vector<ea::instance::Fitness> fitness{};
};

/**
 * @brief The base class of (E|G)A search algorithms.
 */
class Algorithm {
 public:
  virtual ~Algorithm() = default;

  /**
   * @param config algorithm configuration.
   */
  Algorithm(BaseAlgorithmConfig const& config, core::sat::prop::RProp prop);

  /**
   * @brief Performs preparation step.
   */
  void prepare(preprocess::RPreprocess const& preprocess);

  /**
   * @brief Starts the algorithm.
   */
  void process();

  void set_problem(core::sat::Problem& problem);
  core::sat::Problem* _problem = nullptr;

  /**
   * @return the propagation engine used in this algorithm.
   */
  core::sat::prop::Prop& get_prop() noexcept;

  /**
   * @return the best instance currently found.
   */
  instance::Instance& get_best() noexcept;

  /**
   * @return the data shared between instances.
   */
  instance::SharedData& get_shared_data() noexcept;

  /**
   * @return the inaccurate number of visited points.
   */
  [[nodiscard]] uint64_t inaccurate_points() const noexcept;

  /**
   * @return inaccurately checks whether there are points that are not visited.
   */
  [[nodiscard]] bool has_unvisited_points() const noexcept;

  /**
   * @brief Sets the base assumption, i.e. assumption which is always passed
   * to any SAT solver operations.
   * @param assumption the base assumption
   */
  void set_base_assumption(core::lit_vec_t const& assumption) noexcept;

  /**
   * @brief interrupts the algorithm.
   */
  void interrupt();

  /**
   * @return whether the algorithm has been interrupted
   */
  [[nodiscard]] bool is_interrupted() const;

  /**
   * @return algorithms' search statistics
   */
  [[nodiscard]] AlgorithmStatistics const& get_statistics() const noexcept;

 protected:
  void _add_instance();

  virtual void _prepare();

  virtual void _step() = 0;

 protected:
  InstanceConfig _instance_config;
  ea::instance::Population _population;
  ea::instance::RSharedData _shared_data;
  ea::preprocess::RPreprocess _preprocess;
  core::sat::prop::RProp _prop;
  AlgorithmStatistics _stats{};
  bool _save_stats = false;

 private:
  void _init_shared_data(InstanceConfig const& config);

 private:
  ea::limit::RLimit _limit;
  bool _interrupted = false;
};

MAKE_REFS(Algorithm);

DEFINE_REGISTRY(Algorithm, AlgorithmConfig, algorithm, core::sat::prop::RProp);

}  // namespace ea::algorithm

#endif  // EA_BASE_ALGORITHM_H
