#ifndef EVOL_GAALGORITHM_H
#define EVOL_GAALGORITHM_H

#include "evol/include/algorithm/Algorithm.h"
#include "evol/include/method/Selector.h"
#include "evol/include/method/mutate/Mutation.h"
#include "evol/include/util/Registry.h"

namespace ea::algorithm {

class GAAlgorithm : public Algorithm {
 public:
  explicit GAAlgorithm(GAAlgorithmConfig const& config);

  void prepare() override;

  void step() override;

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