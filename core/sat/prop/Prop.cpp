#include "core/sat/prop/Prop.h"

namespace core::sat::prop {

void Prop::_prop_assignments(
    core::domain::USearch assignment_p, prop_callback_t const& callback) {
  sequential_propagate(*this, std::move(assignment_p), callback);
}

void Prop::_prop_assignments(
    lit_vec_t const& base_assumption, core::domain::USearch assignment_p,
    prop_callback_t const& callback) {
  sequential_propagate(
      base_assumption, *this, std::move(assignment_p), callback);
}

bool Prop::_propagate(lit_vec_t const& assumptions) {
  lit_vec_t dummy;
  return _propagate(assumptions, dummy);
}

bool Prop::propagate(lit_vec_t const& assumptions) {
  std::lock_guard<std::mutex> lg(_m);
  return _propagate(assumptions);
}

bool Prop::propagate(lit_vec_t const& assumptions, lit_vec_t& propagated) {
  std::lock_guard<std::mutex> lg(_m);
  return _propagate(assumptions, propagated);
}

uint64_t Prop::prop_tree(lit_vec_t const& vars, uint32_t head_size) {
  std::lock_guard<std::mutex> lg(_m);
  return _prop_tree(vars, head_size);
}

void Prop::prop_assignments(
    domain::USearch search, prop_callback_t const& callback) {
  std::lock_guard<std::mutex> lg(_m);
  _prop_assignments(std::move(search), callback);
}

void Prop::prop_assignments(
    const lit_vec_t& base_assumption, domain::USearch search,
    const Prop::prop_callback_t& callback) {
  std::lock_guard<std::mutex> lg(_m);
  return _prop_assignments(base_assumption, std::move(search), callback);
}

void Prop::sequential_propagate(
    Prop& prop, domain::USearch search_p, const prop_callback_t& callback) {
  domain::Search& search = *search_p;
  do {
    Mini::vec<Mini::Lit> const& assumptions = search();
    if (!callback(prop._propagate(assumptions), assumptions)) {
      break;
    }
  } while (++search);
}

void Prop::sequential_propagate(
    lit_vec_t const& base_assumption, Prop& prop, domain::USearch search_p,
    const prop_callback_t& callback) {
  domain::Search& search = *search_p;
  do {
    auto cur_assumption = util::concat(base_assumption, search());
    if (!callback(prop._propagate(cur_assumption), cur_assumption)) {
      break;
    }
  } while (++search);
}

}  // namespace core::sat::prop