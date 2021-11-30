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
    LOG_TIME(8, step());
    auto& best = get_best();
    VLOG(5) << "Best instance fit: " << best.fitness().rho << " with size: " << best.fitness().pow_r
            << " and fitness: " << (double) best.fitness();
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

instance::Instance& Algorithm::get_best() noexcept {
  return *(*std::min_element(
      population_.begin(), population_.end(), [](auto& a, auto& b) { return *a < *b; }));
}

}  // namespace ea::algorithm
