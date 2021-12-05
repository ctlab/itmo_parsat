#include "evol/include/algorithm/Algorithm.h"

#include "evol/include/util/Registry.h"
#include "evol/include/util/SigHandler.h"
#include "evol/include/util/stream.h"
#include "evol/include/util/Logging.h"

namespace ea::algorithm {

Algorithm::Algorithm() : Algorithm(config::Configuration::get_global_config().algorithm_config()) {}

Algorithm::Algorithm(AlgorithmConfig const& config)
    : solver_(registry::Registry::resolve_solver(config.solver_type()))
    , limit_(registry::Registry::resolve_limit(config.limit_type())) {}

void Algorithm::prepare() {}

void Algorithm::process() {
  size_t iteration = 0;
  limit_->start();
  while (!is_interrupted() && limit_->proceed(population_)) {
    LOG_TIME(step());
    EALOG(LogType::BEST_INSTANCE) << "[Iter " << iteration << "]"
                                  << "[Points " << instance::Instance::inaccurate_points() << "]"
                                  << " Best instance: " << get_best();
    ++iteration;
  }
}

void Algorithm::interrupt() {
  interrupted_ = true;
  solver_->interrupt();
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
