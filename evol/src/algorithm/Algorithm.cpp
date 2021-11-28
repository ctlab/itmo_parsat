#include "evol/include/algorithm/Algorithm.h"

#include "evol/include/util/Registry.h"
#include "evol/include/util/SigHandler.h"

namespace ea::algorithm {

Algorithm::Algorithm() : Algorithm(config::Configuration::get_global_config().algorithm_config()) {}

Algorithm::Algorithm(AlgorithmConfig const& config)
    : solver_(registry::Registry::resolve_solver(config.solver_type()))
    , limit_(registry::Registry::resolve_limit(config.limit_type())) {}

void Algorithm::prepare() {}

void Algorithm::process() {
  limit_->start();
  while (!is_interrupted() && limit_->proceed(population_)) {
    LOG_TIME(step());
    VLOG(4) << "Best instance fit: " << (double) population_.front()->fitness().rho
            << " with size: " << population_.front()->fitness().pow_r;
  }
}

void Algorithm::interrupt() {
  interrupted_ = true;
}

bool Algorithm::is_interrupted() const {
  return interrupted_;
}

sat::Solver& Algorithm::get_solver() noexcept {
  return *solver_;
}

instance::Population& Algorithm::get_population() noexcept {
  return population_;
}

}  // namespace ea::algorithm
