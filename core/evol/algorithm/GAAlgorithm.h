#ifndef EVOL_GAALGORITHM_H
#define EVOL_GAALGORITHM_H

#include "core/evol/algorithm/Algorithm.h"
#include "core/evol/method/select/Selector.h"
#include "core/evol/method/mutate/Mutation.h"
#include "core/evol/method/cross/Crossover.h"
#include "util/Registry.h"

namespace ea::algorithm {

/**
 * @brief Genetic algorithm.
 */
class GAAlgorithm : public Algorithm {
 public:
  explicit GAAlgorithm(
      GAAlgorithmConfig const& config, core::sat::prop::RProp prop);

 protected:
  void _prepare() override;

  void _step() override;

 private:
  void _recalculate_fitness();

 private:
  ea::method::RMutation mutator_;
  ea::method::RCrossover cross_;
  ea::method::RSelector selector_;
  uint32_t q_, h_;
  std::vector<double> fits_;
};

}  // namespace ea::algorithm

#endif  // EVOL_GAALGORITHM_H