#include "core/domain/assignment/Search.h"

namespace core::domain {

Search::Search(uint64_t total) : _total(total) {}

uint64_t Search::size() const noexcept {
  return _total;
}

bool Search::empty() const noexcept {
  return size() == 0;
}

bool Search::operator++() {
  if (_done == _total - 1 || _total == 0) {
    return false;
  } else {
    _advance();
    ++_done;
    return true;
  }
}

void Search::_set_assignment(Minisat::vec<Minisat::Lit>& vec, uint64_t value) {
  for (int i = 0; i < vec.size(); ++i) {
    vec[i] = Minisat::mkLit(Minisat::var(vec[i]), value & (1ULL << i));
  }
}

uint64_t Search::total_size(std::vector<bool> const& vars) {
  return 1ULL << std::count(vars.begin(), vars.end(), true);
}

SingleSearch::SingleSearch() : Search(1) {}

Minisat::vec<Minisat::Lit> const& SingleSearch::operator()() const {
  static Minisat::vec<Minisat::Lit> empty_asgn;
  return empty_asgn;
}

void SingleSearch::_advance() {}

USingleSearch createSingleSearch() {
  return std::make_unique<SingleSearch>();
}

}  // namespace core::domain