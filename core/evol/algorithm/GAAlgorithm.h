#ifndef EVOL_GAALGORITHM_H
#define EVOL_GAALGORITHM_H

#include "core/evol/algorithm/Algorithm.h"
#include "core/evol/method/select/Selector.h"
#include "core/evol/method/mutate/Mutation.h"
#include "core/evol/method/cross/Crossover.h"
#include "core/util/Registry.h"

namespace ea::algorithm {

/**
 * @brief Genetic algorithm.
 */
class GAAlgorithm : public Algorithm {
 public:
  explicit GAAlgorithm(GAAlgorithmConfig const& config);

  void step() override;

 protected:
  void _prepare() override;

 private:
  ea::method::RMutation mutator_;
  ea::method::RCrossover cross_;
  ea::method::RSelector selector_;
  uint32_t q_, h_;

 private:
  void _recalc_fits();
  std::vector<double> fits_;
};

}  // namespace ea::algorithm

#endif  // EVOL_GAALGORITHM_H