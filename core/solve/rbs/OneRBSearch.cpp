#include "core/solve/rbs/OneRBSearch.h"

namespace core::solve {

OneRBSearch::OneRBSearch(
    OneRBSearchConfig config, sat::prop::RProp prop,
    ea::preprocess::RPreprocess preprocess)
    : RBSearch(std::move(prop), std::move(preprocess))
    , _cfg(std::move(config)) {}

void OneRBSearch::_interrupt_impl() {
  _interrupt(_algorithm);
}

rbs_result_t OneRBSearch::find_rb() {
  _algorithm.reset(ea::algorithm::AlgorithmRegistry::resolve(
      _cfg.algorithm_config(), _prop));
  IPS_TRACE(_algorithm->prepare(_preprocess));
  IPS_TRACE(_algorithm->process());
  rbs_result_t result;
  if (IPS_UNLIKELY(_is_interrupted())) {
    result = RBS_INTERRUPTED;
  } else {
    auto rb = _algorithm->get_best();
    IPS_INFO("The best rho-backdoor:\n\t" << rb);
    if (rb.is_sbs()) {
      result = RBS_SBS_FOUND;
    } else {
      result = core::search::createFullSearch(
          _preprocess->var_view(), rb.get_vars().get_mask());
    }
  }
  _algorithm.reset();
  return result;
}

REGISTER_PROTO(RBSearch, OneRBSearch, one_rb_search_config);

}  // namespace core::solve