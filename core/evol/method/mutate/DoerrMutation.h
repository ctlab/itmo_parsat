#ifndef EVOL_DOERRMUTATION_H
#define EVOL_DOERRMUTATION_H

#include "core/evol/method/mutate/Mutation.h"

namespace ea::method {

/**
 * @brief Doerr-optimized mutation.
 */
class DoerrMutation : public Mutation {
 public:
  explicit DoerrMutation(DoerrMutationConfig const& config);

  void apply(instance::Instance& instance) override;

 private:
  [[nodiscard]] uint32_t _get_alpha(uint32_t size) const;

 private:
  double beta_;
  uint32_t alpha_ = 0;
};

}  // namespace ea::method

#endif  // EVOL_DOERRMUTATION_H
