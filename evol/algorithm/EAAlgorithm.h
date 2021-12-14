#ifndef EVOL_EAALGORITHM_H
#define EVOL_EAALGORITHM_H

#include "evol/algorithm/Algorithm.h"
#include "evol/method/select/Selector.h"
#include "evol/method/mutate/Mutation.h"
#include "core/util/Registry.h"

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
