#include "core/solve/rbs/RBSearch.h"

namespace core::solve {

RBSearch::RBSearch(
    sat::prop::RProp prop, ea::preprocess::RPreprocess preprocess)
    : _prop(std::move(prop)), _preprocess(std::move(preprocess)) {}

void RBSearch::interrupt() {
  _interrupted = true;
  _interrupt_impl();
}

bool RBSearch::_is_interrupted() const noexcept {
  return _interrupted;
}

}  // namespace core::solve