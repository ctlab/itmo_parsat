#ifndef EA_BASE_ALGORITHM_H
#define EA_BASE_ALGORITHM_H

#include <utility>

#include "evol/include/algorithm/Algorithm.h"
#include "evol/include/domain/Instance.h"
#include "evol/include/limit/Limit.h"
#include "evol/include/method/Generator.h"
#include "evol/include/method/Selector.h"
#include "evol/include/util/profile.h"
#include "evol/proto/config.pb.h"

namespace ea::algorithm {

/**
 * The general implementation of EA pipeline
 */
class BaseAlgorithm : public Algorithm {
 public:
  virtual ~BaseAlgorithm() = default;

  /**
   * Creates algorithm from configuration
   */
  BaseAlgorithm();

  /**
   * Algorithm is parametrized by:
   *  - generator, an algorithm that generates instances for next pop.
   *  - selector, an algorithm that selects next population.
   *  - limit controls the termination of an algorithm.
   */
  explicit BaseAlgorithm(BaseAlgorithmConfig const& config);

  /**
   * Starts algorithm.
   */
  void process() override;

  /**
   * Sets the population to the desired one.
   */
  void set_population(instance::RPopulation population);

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

#endif  // EA_BASE_ALGORITHM_H
