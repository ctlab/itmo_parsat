#ifndef EVOL_MPLALGORITHM_H
#define EVOL_MPLALGORITHM_H

#include "evol/include/algorithm/Algorithm.h"
#include "evol/include/method/Selector.h"
#include "evol/include/method/mutate/Mutation.h"
#include "evol/include/util/Registry.h"

namespace ea::algorithm {

class MpLAlgorithm : public Algorithm {
 public:
  explicit MpLAlgorithm(MpLAlgorithmConfig const& config);

  void prepare() override;

  void step() override;

 private:
  ea::method::RMutation mutator_;
  ea::method::RCrossover cross_;
  ea::method::RSelector selector_;
  uint32_t mu_, lambda_;
};

}  // namespace ea::method

#endif  // EVOL_MPLALGORITHM_H