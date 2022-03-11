#ifndef ITMO_PARSAT_PREPROCESS_H
#define ITMO_PARSAT_PREPROCESS_H

#include "core/sat/Problem.h"
#include "core/sat/prop/SimpProp.h"
#include "core/domain/VarView.h"

namespace ea::preprocess {

struct Preprocess {
 public:
  explicit Preprocess(PreprocessConfig const& config, core::sat::Problem const& problem);

  core::domain::VarView const& var_view() const noexcept;

 private:
  core::domain::VarView _var_view;
};

MAKE_REFS(Preprocess);

}  // namespace ea::preprocess

#endif  // ITMO_PARSAT_PREPROCESS_H
