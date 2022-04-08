#include "core/search/RandomSearch.h"

namespace {

void set_random(core::lit_vec_t& vec) {
  for (int i = 0; i < vec.size(); ++i) {
    vec[i] = Mini::mkLit(Mini::var(vec[i]), ::util::random::sample<int>(0, 1));
  }
}

}  // namespace

namespace core::search {

RandomSearch::RandomSearch(
    domain::VarView const& var_view, bit_mask_t const& bit_mask, uint64_t total)
    : Search(var_view, bit_mask, total) {
  set_random(_assignment);
}

RandomSearch::RandomSearch(std::vector<int> const& vars, uint64_t total)
    : Search(vars, total) {
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

USearch createRandomSearch(
    domain::VarView const& var_view, bit_mask_t const& bit_mask,
    uint64_t total) {
  return USearch(new RandomSearch(var_view, bit_mask, total));
}

USearch createRandomSearch(std::vector<int> const& vars, uint64_t total) {
  return USearch(new RandomSearch(vars, total));
}

USearch createAutoRandomSearch(
    domain::VarView const& var_view, bit_mask_t const& bit_mask,
    uint64_t total) {
  uint32_t num_set = std::count(bit_mask.begin(), bit_mask.end(), true);
  if (num_set <= domain::SearchSpace::MAX_VARS_FOR_FULL_SEARCH) {
    return USearch(new UniqueSearch(var_view, bit_mask, total));
  } else {
    return USearch(new RandomSearch(var_view, bit_mask, total));
  }
}

USearch createAutoRandomSearch(std::vector<int> const& vars, uint64_t total) {
  if (vars.size() <= domain::SearchSpace::MAX_VARS_FOR_FULL_SEARCH) {
    return USearch(new UniqueSearch(vars, total));
  } else {
    return USearch(new RandomSearch(vars, total));
  }
}

}  // namespace core::search
