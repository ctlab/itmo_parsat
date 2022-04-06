#include "core/sat/prop/Prop.h"

namespace core::sat::prop {

bool Prop::propagate(lit_vec_t const& assumptions) {
  lit_vec_t dummy;
  return propagate(assumptions, dummy);
}

uint64_t Prop::prop_search(
    search::USearch search, prop_callback_t const& callback) {
  return sequential_propagate(*this, std::move(search), callback);
}

uint64_t Prop::prop_search(
    lit_vec_t const& base_assumption, search::USearch search,
    prop_callback_t const& callback) {
  return sequential_propagate(
      *this, base_assumption, std::move(search), callback);
}

uint64_t Prop::sequential_propagate(
    Prop& prop, search::USearch search_p, const prop_callback_t& callback) {
  search::Search& search = *search_p;
  uint64_t conflicts = 0;
  do {
    Mini::vec<Mini::Lit> const& assumptions = search();
    bool conflict = prop.propagate(assumptions);
    conflicts += conflict;
    if (callback) {
      try {
        if (!callback(conflict, assumptions)) {
          break;
        }
      } catch (...) {
        // ignore
      }
    }
  } while (++search);
  return conflicts;
}

uint64_t Prop::sequential_propagate(
    Prop& prop, lit_vec_t const& base_assumption, search::USearch search_p,
    prop_callback_t const& callback) {
  search::Search& search = *search_p;
  uint64_t conflicts = 0;
  do {
    auto cur_assumption = util::concat(base_assumption, search());
    bool conflict = prop.propagate(cur_assumption);
    conflicts += conflict;
    if (callback) {
      try {
        if (!callback(conflict, cur_assumption)) {
          break;
        }
      } catch (...) {
        // ignore
      }
    }
  } while (++search);
  return conflicts;
}

}  // namespace core::sat::prop