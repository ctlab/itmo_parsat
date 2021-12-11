#ifndef EVOL_EAALGORITHM_H
#define EVOL_EAALGORITHM_H

#include "evol/include/algorithm/Algorithm.h"
#include "evol/include/method/select/Selector.h"
#include "evol/include/method/mutate/Mutation.h"
#include "evol/include/util/Registry.h"

namespace ea::algorithm {

class EAAlgorithm : public Algorithm {
 public:
  explicit EAAlgorithm(EAAlgorithmConfig const& config);

  void step() override;

 protected:
  void _prepare() override;

 private:
  ea::method::RMutation mutator_;
  ea::method::RSelector selector_;
};

}  // namespace ea::algorithm

#endif  // EVOL_EAALGORITHM_H
