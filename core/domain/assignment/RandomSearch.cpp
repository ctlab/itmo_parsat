#include "core/domain/assignment/RandomSearch.h"
#include "core/domain/SearchSpace.h"
#include "UniqueSearch.h"

namespace {

void set_random(core::lit_vec_t& vec) {
  for (int i = 0; i < vec.size(); ++i) {
    vec[i] = Mini::mkLit(Mini::var(vec[i]), ::util::random::sample<int>(0, 1));
  }
}

}  // namespace

namespace core::domain {

RandomSearch::RandomSearch(
    VarView const& var_view, bit_mask_t const& bit_mask, uint64_t total)
    : AssignmentModifier(var_view, bit_mask), Search(total) {
  set_random(_assignment);
}

RandomSearch::RandomSearch(std::vector<int> const& vars, uint64_t total)
    : AssignmentModifier(vars), Search(total) {
  set_random(_assignment);
}

void RandomSearch::_advance() {
  set_random(_assignment);
}

void RandomSearch::_reset() {
  _advance();
}

RandomSearch* RandomSearch::clone() const {
  return new RandomSearch(*this);
}

lit_vec_t const& RandomSearch::operator()() const {
  return get();
}

USearch createRandomSearch(
    VarView const& var_view, bit_mask_t const& bit_mask, uint64_t total) {
  uint32_t num_set = std::count(bit_mask.begin(), bit_mask.end(), true);
  if (num_set <= SearchSpace::MAX_VARS_FOR_FULL_SEARCH) {
    return USearch(new UniqueSearch(var_view, bit_mask, total));
  } else {
    return USearch(new RandomSearch(var_view, bit_mask, total));
  }
}

USearch createRandomSearch(std::vector<int> const& vars, uint64_t total) {
  uint32_t num_set = std::count(vars.begin(), vars.end(), true);
  if (num_set <= SearchSpace::MAX_VARS_FOR_FULL_SEARCH) {
    return USearch(new UniqueSearch(vars, total));
  } else {
    return USearch(new RandomSearch(vars, total));
  }
}

}  // namespace core::domain
