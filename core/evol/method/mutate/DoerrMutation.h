#ifndef EVOL_DOERRMUTATION_H
#define EVOL_DOERRMUTATION_H

#include "core/evol/method/mutate/Mutation.h"

namespace ea::method {

class DoerrMutation : public Mutation {
 public:
  explicit DoerrMutation(DoerrMutationConfig const& config);

  void apply(instance::Instance& instance) override;

 private:
  [[nodiscard]] double _get_alpha(size_t size) const;

 private:
  double beta_;
  double alpha_ = -1.;
};

}  // namespace ea::method

#endif  // EVOL_DOERRMUTATION_H
