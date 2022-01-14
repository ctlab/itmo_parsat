#ifndef EVOL_EAALGORITHM_H
#define EVOL_EAALGORITHM_H

#include "core/evol/algorithm/Algorithm.h"
#include "core/evol/method/select/Selector.h"
#include "core/evol/method/mutate/Mutation.h"
#include "core/util/Registry.h"

namespace ea::algorithm {

/**
 * @brief Evolutionary (1+1) algorithm.
 */
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
