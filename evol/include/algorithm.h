#ifndef EA_ALGORITHM_H
#define EA_ALGORITHM_H

#include "evol/include/instance.h"
#include "evol/include/selector.h"
#include "evol/include/generation.h"
#include "evol/include/limit.h"
#include "evol/include/types.h"
#include "evol/include/util/profile.h"


namespace ea::algorithm {

class BaseAlgorithm {
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
        types::RPopulation initial_population,
        generation::RGeneration generator,
        selector::RSelector selector,
        limit::RLimit limit
      ) 
    : population_(initial_population),
      generator_(generator),
      selector_(selector),
      limit_(limit) {
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

} // namespace ea::algorithm


#endif // EA_ALGORITHM_H
