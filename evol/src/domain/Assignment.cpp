#include "evol/include/domain/Assignment.h"

#include "evol/include/util/random.h"

namespace ea::domain {

Assignment::Assignment(std::map<int, int> const& var_map, std::vector<bool> const& vars) {
  for (int i = 0; i < (int) vars.size(); ++i) {
    if (vars[i]) {
      assignment_.push(Minisat::mkLit(var_map.at(i), false));
    }
  }
}

Minisat::vec<Minisat::Lit> const& Assignment::operator()() const {
  return assignment_;
}

FullSearch::FullSearch(std::map<int, int> const& var_map, std::vector<bool> const& vars)
    : Assignment(var_map, vars) {}

Assignment* FullSearch::clone() const {
  return new FullSearch(*this);
}

void FullSearch::set(uint32_t index) {
  for (int pos = 0; pos < assignment_.size(); ++pos) {
    bool sign = index & (1ULL << pos);
    assignment_[pos] = Minisat::mkLit(Minisat::var(assignment_[pos]), sign);
  }
}

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

uint32_t FullSearch::size() const noexcept {
  return (uint32_t) std::pow(2, assignment_.size());
}

RandomAssignments::RandomAssignments(
    std::map<int, int> const& var_map, std::vector<bool> const& vars, uint32_t total)
    : Assignment(var_map, vars), total_(total), left_(total + 1) {
  ++(*this);
}

Assignment* RandomAssignments::clone() const {
  return new RandomAssignments(*this);
}

void RandomAssignments::set(uint32_t index) {
  ++(*this);
}

bool RandomAssignments::operator++() {
  if (--left_ == 0) {
    return false;
  }
  for (int i = 0; i < assignment_.size(); ++i) {
    assignment_[i] = Minisat::mkLit(Minisat::var(assignment_[i]), random::sample<int>(0, 1));
  }
  return true;
}

uint32_t RandomAssignments::size() const noexcept {
  return total_;
}

}  // namespace ea::domain
