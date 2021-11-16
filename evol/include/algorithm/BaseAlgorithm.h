#ifndef EA_BASE_ALGORITHM_H
#define EA_BASE_ALGORITHM_H

#include <utility>

#include "evol/include/algorithm/Algorithm.h"
#include "evol/include/domain/Instance.h"
#include "evol/include/method/Generator.h"
#include "evol/include/method/Selector.h"
#include "evol/include/util/Builder.h"
#include "evol/include/util/Limit.h"
#include "evol/include/util/profile.h"

namespace ea::algorithm {

/**
 * The general implementation of EA pipeline
 */
class BaseAlgorithm : public Algorithm {
 public:
  virtual ~BaseAlgorithm() = default;

  /**
   * Algorithm is parametrized by:
   *  - initial population (can be empty if generator supports it).
   *  - generator, an algorithm that generates instances for next pop.
   *  - selector, an algorithm that selects next population.
   *  - limit controls the termination of an algorithm.
   */
  BaseAlgorithm(
      instance::RPopulation population, generator::RGeneration generator,
      selector::RSelector selector, limit::RLimit limit);

  /**
   * Starts algorithm.
   */
  void process() override;

 protected:
  /**
   * Basic implementation of EA step.
   * For more complex implementations, override this method.
   */
  virtual void step();

 private:
  instance::RPopulation population_;
  generator::RGeneration generator_;
  selector::RSelector selector_;
  limit::RLimit limit_;
};

}  // namespace ea::algorithm

namespace ea::builder {

class BaseAlgorithmBuilder : public Builder<algorithm::Algorithm> {
 public:
  BaseAlgorithmBuilder() = default;

  void set_population(instance::RPopulation population);

  void set_generator(generator::RGeneration generator);

  void set_selector(selector::RSelector selector);

  void set_limit(limit::RLimit limit);

  algorithm::Algorithm* build() override;

  char const** get_params() override;

 private:
  instance::RPopulation population_;
  generator::RGeneration generator_;
  selector::RSelector selector_;
  limit::RLimit limit_;
};

}  // namespace ea::builder

#endif  // EA_BASE_ALGORITHM_H
