#include "Sharing.h"

namespace core::sat::sharing {

Sharing::Sharing(int interval_us, int shr_lit)
    : _interval_us(interval_us), _shr_lit(shr_lit) {}

void Sharing::stop() noexcept {
  _sharers.clear();
}

void Sharing::share(Shareable& s1, Shareable& s2, SharingDir dir) {
  share(s1.sharing_unit(), s2.sharing_unit(), dir);
}

void Sharing::share(SolverBlockList const& su) {
  std::vector<SolverInterface*> consumers;
  for (auto const& block : su) {
    auto const& [solvers, _] = block;
    // solvers are always consumers
    consumers.insert(consumers.end(), solvers.begin(), solvers.end());
  }
  for (auto const& unit : su) {
    auto [producers, reducer] = unit;

    // reducer may be null for short blocks
    if (reducer != nullptr) {
      // add the current reducer to consumers and producers
      producers.push_back(reducer);
      consumers.push_back(reducer);
    }

    _add_sharer(producers, consumers);

    if (reducer != nullptr) {
      consumers.pop_back();
    }
  }
}

void Sharing::share(SolverList const& sl) {
  _add_sharer(sl, sl);
}

void Sharing::share(SharingUnit const& u) {
  std::visit(
      overloaded{
          [this](SolverList const& sl) { share(sl); },
          [this](SolverBlockList const& sl) { share(sl); }},
      u);
}

void Sharing::share(
    SharingUnit const& su1, SharingUnit const& su2, SharingDir dir) {
  if (dir == RIGHT || dir == BIDIR) {
    _share_right(su1, su2);
  }
  if (dir == LEFT || dir == BIDIR) {
    _share_right(su2, su1);
  }
}

void Sharing::_share_right(SharingUnit const& su1, SharingUnit const& su2) {
  auto sl1 = get_all_solvers(su1);
  auto sl2 = get_all_solvers(su2);
  _add_sharer(sl1, sl2);
}

void Sharing::_add_sharer(SolverList const& prod, SolverList const& cons) {
  _sharers.push_back(std::make_unique<painless::Sharer>(
      _interval_us, 0, new painless::HordeSatSharing(_shr_lit, _interval_us),
      prod, cons));
}

SolverList Sharing::get_all_solvers(SharingUnit const& su) {
  return std::visit(
      overloaded{
          [](SolverList const& sl) { return sl; },
          [](SolverBlockList const& sbl) {
            SolverList result;
            for (auto const& [solvers, reducer] : sbl) {
              result.insert(result.end(), solvers.begin(), solvers.end());
              if (reducer != nullptr) {
                result.push_back(reducer);
              }
            }
            return result;
          }},
      su);
}

}  // namespace core::sat::sharing
