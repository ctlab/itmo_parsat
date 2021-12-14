#ifndef EVOL_GAALGORITHM_H
#define EVOL_GAALGORITHM_H

#include "evol/algorithm/Algorithm.h"
#include "evol/method/select/Selector.h"
#include "evol/method/mutate/Mutation.h"
#include "evol/method/cross/Crossover.h"
#include "core/util/Registry.h"

namespace ea::algorithm {

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