#include "evol/include/domain/Instance.h"

#include <cmath>

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
  return (double) a < (double) b;
}

bool operator<(Instance& a, Instance& b) {
  return a.fitness() < b.fitness();
}

Assignment::Assignment(std::set<unsigned int> const& variables) {
  for (unsigned x : variables) {
    asgn_.push(Minisat::mkLit((int) x, false));
  }
}

Minisat::vec<Minisat::Lit> const& Assignment::operator()() const {
  return asgn_;
}

bool Assignment::operator++() {
  int pos = 0;
  while (pos < asgn_.size() && sign(asgn_[pos])) {
    asgn_[pos] = ~asgn_[pos];
    ++pos;
  }
  if (pos == asgn_.size()) {
    return false;
  }
  asgn_[pos] = Minisat::mkLit(pos, true);
  return true;
}

}  // namespace ea::instance