#ifndef EVOL_OPOALGORITHM_H
#define EVOL_OPOALGORITHM_H

#include "evol/include/algorithm/Algorithm.h"
#include "evol/include/method/Selector.h"
#include "evol/include/method/mutate/Mutation.h"
#include "evol/include/util/Registry.h"

namespace ea::algorithm {

class OpOAlgorithm : public Algorithm {
 public:
  explicit OpOAlgorithm(OpOAlgorithmConfig const& config);

  void prepare() override;

  void step() override;

 private:
  ea::method::RMutation mutator_;
  ea::method::RSelector selector_;
};

}  // namespace ea::method

#endif  // EVOL_OPOALGORITHM_H
