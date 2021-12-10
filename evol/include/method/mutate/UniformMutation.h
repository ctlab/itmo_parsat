#ifndef EVOL_UNIFORMMUTATION_H
#define EVOL_UNIFORMMUTATION_H

#include "evol/include/method/mutate/Mutation.h"

namespace ea::method {

class UniformMutation : public Mutation {
 public:
  explicit UniformMutation(UniformMutationConfig const& config);

  void apply(domain::Instance& instance) override;

 private:
  double scale_;
};

}  // namespace ea::method

#endif  // EVOL_UNIFORMMUTATION_H
