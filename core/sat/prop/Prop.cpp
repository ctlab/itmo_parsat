#include "core/sat/prop/Prop.h"

namespace core::sat::prop {

void Prop::_prop_assignments(
    core::domain::USearch assignment_p, prop_callback_t const& callback) {
  sequential_propagate(*this, std::move(assignment_p), callback);
}

void Prop::_prop_assignments(
    const Mini::vec<Mini::Lit>& base_assumption,
    core::domain::USearch assignment_p, prop_callback_t const& callback) {
  sequential_propagate(
      base_assumption, *this, std::move(assignment_p), callback);
}

bool Prop::_propagate(Mini::vec<Mini::Lit> const& assumptions) {
  Mini::vec<Mini::Lit> props;
  return _propagate(assumptions, props);
}

bool Prop::propagate(Mini::vec<Mini::Lit> const& assumptions) {
  std::lock_guard<std::mutex> lg(_m);
  return _propagate(assumptions);
}

bool Prop::propagate(
    Mini::vec<Mini::Lit> const& assumptions, vec_lit_t& propagated) {
  std::lock_guard<std::mutex> lg(_m);
  return _propagate(assumptions, propagated);
}

uint64_t Prop::prop_tree(vec_lit_t const& vars, uint32_t head_size) {
  std::lock_guard<std::mutex> lg(_m);
  return _prop_tree(vars, head_size);
}

void Prop::prop_assignments(
    domain::USearch search, const prop_callback_t& callback) {
  std::lock_guard<std::mutex> lg(_m);
  _prop_assignments(std::move(search), callback);
}

void Prop::prop_assignments(
    const Mini::vec<Mini::Lit>& base_assumption, domain::USearch search,
    const Prop::prop_callback_t& callback) {
  std::lock_guard<std::mutex> lg(_m);
  return _prop_assignments(base_assumption, std::move(search), callback);
}

void Prop::sequential_propagate(
    Prop& prop, domain::USearch search, const prop_callback_t& callback) {
  domain::Search& assignment = *search;
  START_ASGN_TRACK(search->size());
  do {
    Mini::vec<Mini::Lit> const& assumptions = assignment();
    ASGN_TRACK(assumptions);
    if (!callback(prop._propagate(assumptions), assumptions)) {
      BREAK_ASGN_TRACK;
      break;
    }
  } while (++assignment);
  END_ASGN_TRACK(false);
}

void Prop::sequential_propagate(
    const Mini::vec<Mini::Lit>& base_assumption, Prop& prop,
    domain::USearch search, const prop_callback_t& callback) {
  domain::Search& assignment = *search;
  START_ASGN_TRACK(search->size());
  do {
    Mini::vec<Mini::Lit> cur_assumption = base_assumption;
    {
      Mini::vec<Mini::Lit> const& assumptions = assignment();
      cur_assumption.capacity(cur_assumption.size() + assumptions.size());
      for (int i = 0; i < assumptions.size(); ++i) {
        cur_assumption.push(assumptions[i]);
      }
    }
    ASGN_TRACK(cur_assumption);
    if (!callback(prop._propagate(cur_assumption), cur_assumption)) {
      BREAK_ASGN_TRACK;
      break;
    }
  } while (++assignment);
  END_ASGN_TRACK(false);
}

}  // namespace core::sat::prop