#include "evol/include/domain/Instance.h"

#include <glog/logging.h>

#include <cmath>

#include "evol/include/util/random.h"

namespace ea::instance {

bool Fitness::can_calc() const {
  /* 55 is just some value less than 63 */
  return pow_r < 55 && pow_nr < 55;
}

Fitness::operator double() const {
  return rho * std::pow(2., pow_r) + (1. - rho) * std::pow(2., pow_nr);
}

bool operator<(Fitness const& a, Fitness const& b) {
  /* NOTE: For now, pow_nr is always equal to 20 */
  if (a.can_calc() && b.can_calc()) {
    return (double) a < (double) b;
  }

  /* TODO: assure this will work AT LEAST in most cases. */
  int32_t min_pow_r = std::min(a.pow_r, b.pow_r);
  double a_val = a.rho * std::pow(2., a.pow_r - min_pow_r);
  double b_val = b.rho * std::pow(2., b.pow_r - min_pow_r);

  int32_t pow_nr = a.pow_nr - min_pow_r;
  if (min_pow_r > -60) {
    a_val += (1. - a.rho) * std::pow(2., pow_nr);
    b_val += (1. - b.rho) * std::pow(2., pow_nr);
  }

  return a_val < b_val;
}

bool operator<(Instance& a, Instance& b) {
  return a.fitness() < b.fitness();
}

Assignment::Assignment(std::map<int, int>& var_map, const std::vector<bool>& vars) {
  for (size_t i = 0; i < vars.size(); ++i) {
    if (vars[i]) {
      assignment_.push(Minisat::mkLit((int) var_map[(int) i], false));
    }
  }
}

Minisat::vec<Minisat::Lit> const& Assignment::operator()() const {
  return assignment_;
}

FullSearch::FullSearch(std::map<int, int>& var_map, const std::vector<bool>& vars)
  : Assignment(var_map, vars) {}

bool FullSearch::operator++() {
  int pos = 0;
  while (pos < assignment_.size() && sign(assignment_[pos])) {
    assignment_[pos] = ~assignment_[pos];
    ++pos;
  }
  if (pos == assignment_.size()) {
    return false;
  }
  assignment_[pos] = Minisat::mkLit(pos, true);
  return true;
}

RandomAssignments::RandomAssignments(std::map<int, int>& var_map, const std::vector<bool>& vars)
    : Assignment(var_map, vars) {
  ++(*this);
}

bool RandomAssignments::operator++() {
  for (int i = 0; i < assignment_.size(); ++i) {
    if (random::flip_coin(0.5)) {
      assignment_[i] = ~assignment_[i];
    }
  }
  return true;
}

std::map<int, int> Instance::var_map{};

}  // namespace ea::instance