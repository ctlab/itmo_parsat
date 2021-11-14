#ifndef EA_BASE_ALGORITHM_H
#define EA_BASE_ALGORITHM_H

#include "evol/include/algorithm.h"
#include "evol/include/builder.h"
#include "evol/include/generation.h"
#include "evol/include/instance.h"
#include "evol/include/limit.h"
#include "evol/include/selector.h"
#include "evol/include/types.h"
#include "evol/include/util/profile.h"

namespace ea::algorithm {

class BaseAlgorithm : public IAlgorithm {
 public:
  virtual ~BaseAlgorithm() = default;

  /**
   * Algorithm is parametrized by:
   *  - initial population (can be empty if generator supports it).
   *  - generator, an altorithm that generates instances for next pop.
   *  - selector, an algorithm that selects next population.
   *  - limit controls the termination of an algorithm.
   */
  BaseAlgorithm(
      types::RPopulation population, generation::RGeneration generator,
      selector::RSelector selector, limit::RLimit limit)
      : population_(population), generator_(generator), selector_(selector), limit_(limit) {
  }

  /**
   * Starts algorithm.
   */
  void process() {
    limit_->start();
    while (limit_->proceed()) {
      LOG_TIME(step());
    }
  }

 protected:
  /**
   * Basic implementation of EA step.
   * For more complex implementations, override this method.
   */
  virtual void step() {
    generator_->generate(*population_);
    selector_->select(*population_);
  }

 private:
  types::RPopulation population_;
  generation::RGeneration generator_;
  selector::RSelector selector_;
  limit::RLimit limit_;
};

class BaseAlgorithmBuilder : public builder::Builder<IAlgorithm> {
 public:
  BaseAlgorithmBuilder() = default;

  void set_population(types::RPopulation population);

  void set_generator(generation::RGeneration generator);

  void set_selector(selector::RSelector selector);

  void set_limit(limit::RLimit limit);

  virtual IAlgorithm* build() override;

  virtual char const** get_params() override;

 private:
  types::RPopulation population_;
  generation::RGeneration generator_;
  selector::RSelector selector_;
  limit::RLimit limit_;
};

}  // namespace ea::algorithm

#endif  // EA_BASE_ALGORITHM_H
