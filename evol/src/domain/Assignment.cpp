#include "evol/include/domain/Assignment.h"

#include "evol/include/util/random.h"
#include <glog/logging.h>

namespace {

uint64_t range_max(size_t num_vars) {
  return num_vars == ::ea::domain::Assignment::MAX_VARS_FULL_SEARCH
             ? std::numeric_limits<uint64_t>::max()
             : ((1ULL << num_vars) - 1);
}

void set_assignment(Minisat::vec<Minisat::Lit>& vec, uint64_t value) {
  for (int i = 0; i < vec.size(); ++i) {
    vec[i] = Minisat::mkLit(Minisat::var(vec[i]), value & (1ULL << i));
  }
}

void set_random(Minisat::vec<Minisat::Lit>& vec) {
  for (int i = 0; i < vec.size(); ++i) {
    vec[i] = Minisat::mkLit(Minisat::var(vec[i]), ::ea::random::sample<int>(0, 1));
  }
}

void next_assignment(Minisat::vec<Minisat::Lit>& vec) {
  int pos = 0;
  while (pos < vec.size() && sign(vec[pos])) {
    vec[pos] = ~vec[pos];
    ++pos;
  }
  if (pos < vec.size()) {
    vec[pos] = Minisat::mkLit(pos, true);
  }
}

// clang-format off
void get_start_and_todo(
    uint64_t total,
    uint64_t num_split, uint64_t index,
    uint64_t& start, uint64_t& to_do) {
  uint64_t div = total / num_split;
  uint64_t rem = total % num_split;

  if (index < rem) {
    to_do = div + 1;
    start = index * to_do;
  } else {
    to_do = div;
    start = rem * (div + 1) + (index - rem) * div;
  }
}
// clang-format on

}  // namespace

namespace ea::domain {

Assignment::Assignment(
    std::map<int, int> const& var_map, std::vector<bool> const& vars, uint64_t total)
    : total_(total) {
  for (int i = 0; i < (int) vars.size(); ++i) {
    if (vars[i]) {
      assignment_.push(Minisat::mkLit(var_map.at(i), false));
    }
  }
}

bool Assignment::is_empty() const {
  return total_ == 0;
}

Minisat::vec<Minisat::Lit> const& Assignment::operator()() const {
  return assignment_;
}

bool Assignment::operator++() {
  if (done_ == total_ - 1 || total_ == 0) {
    return false;
  } else {
    _advance();
    ++done_;
    return true;
  }
}

uint64_t Assignment::total() const noexcept {
  return total_;
}

uint64_t SmallSearch::_set_range() {
  LOG_IF(FATAL, assignment_.size() > MAX_VARS_FULL_SEARCH)
      << "Small search can be used only for small assignments!";
  first_ = 0;
  last_ = range_max(assignment_.size());
  return last_ - first_ + 1;
}

SmallSearch::SmallSearch(std::map<int, int> const& var_map, std::vector<bool> const& vars)
    : Assignment(var_map, vars, 0) {
  total_ = _set_range();
}

SmallSearch::SmallSearch(
    std::map<int, int> const& var_map, std::vector<bool> const& vars, uint64_t total)
    : Assignment(var_map, vars, total) {
  _set_range();
}

UAssignment SmallSearch::split_search(uint64_t num_split, uint64_t index) const {
  auto* result = clone();
  uint64_t start, to_do;
  get_start_and_todo(total_, num_split, index, start, to_do);
  if (start < total_) {
    result->first_ = start;
    result->last_ = start + to_do - 1;
    result->done_ = 0;
    result->total_ = to_do;
    result->_reset();
  } else {
    result->total_ = 0;
  }
  return UAssignment(result);
}

RandomSearch::RandomSearch(
    std::map<int, int> const& var_map, std::vector<bool> const& vars, uint64_t total)
    : Assignment(var_map, vars, total) {
  set_random(assignment_);
}

UAssignment RandomSearch::split_search(uint64_t num_split, uint64_t index) const {
  auto* result = new RandomSearch(*this);
  result->total_ = (total_ / num_split) + (index < (total_ % num_split));
  return UAssignment(result);
}

void RandomSearch::_advance() {
  set_random(assignment_);
}

FullSearch::FullSearch(std::map<int, int> const& var_map, std::vector<bool> const& vars)
    : SmallSearch(var_map, vars) {}

void FullSearch::_advance() {
  next_assignment(assignment_);
}

SmallSearch* FullSearch::clone() const {
  return new FullSearch(*this);
}

void FullSearch::_reset() {
  set_assignment(assignment_, first_);
}

UniqueSearch::UniqueSearch(
    const std::map<int, int>& var_map, const std::vector<bool>& vars, uint64_t total)
    : SmallSearch(var_map, vars, total) {
  _advance_us();
}

UniqueSearch::UniqueSearch(
    const std::map<int, int>& var_map, const std::vector<bool>& vars)
    : SmallSearch(var_map, vars) {
  _advance_us();
}

void UniqueSearch::_advance() {
  _advance_us();
}

void UniqueSearch::_advance_us() {
  uint64_t sample = first_;
  while (visited_.count(sample)) {
    sample = random::sample<uint64_t>(first_ + 1, last_);
  }
  visited_.insert(sample);
  set_assignment(assignment_, sample);
}

SmallSearch* UniqueSearch::clone() const {
  return new UniqueSearch(*this);
}

void UniqueSearch::_reset() {
  visited_.clear();
  _advance();
}

UAssignment createFullSearch(std::map<int, int> const& var_map, std::vector<bool> const& vars) {
  return UAssignment(new FullSearch(var_map, vars));
}

UAssignment createRandomSearch(
    std::map<int, int> const& var_map, std::vector<bool> const& vars, uint64_t total) {
  size_t num_set = std::count(vars.begin(), vars.end(), true);
  if (num_set <= Assignment::MAX_VARS_FULL_SEARCH) {
    return UAssignment(new UniqueSearch(var_map, vars, total));
  } else {
    return UAssignment(new RandomSearch(var_map, vars, total));
  }
}

}  // namespace ea::domain
