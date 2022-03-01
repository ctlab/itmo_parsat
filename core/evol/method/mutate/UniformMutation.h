#ifndef EVOL_UNIFORMMUTATION_H
#define EVOL_UNIFORMMUTATION_H

#include "core/evol/method/mutate/Mutation.h"

namespace ea::method {

/**
 * @brief Standard uniform mutation.
 */
class UniformMutation : public Mutation {
 public:
  explicit UniformMutation(UniformMutationConfig const& config);

  void apply(instance::Instance& instance) override;

 private:
  uint32_t scale_;
};

}  // namespace ea::method

#endif  // EVOL_UNIFORMMUTATION_H
