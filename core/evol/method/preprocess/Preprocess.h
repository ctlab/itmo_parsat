#ifndef ITMO_PARSAT_PREPROCESS_H
#define ITMO_PARSAT_PREPROCESS_H

#include "core/sat/Problem.h"
#include "core/sat/prop/SimpProp.h"
#include "core/domain/VarView.h"

namespace ea::preprocess {

/**
 * @brief The class managing sat formula preprocessing and (E|G)A heuristics.
 */
struct Preprocess {
 public:
  explicit Preprocess(PreprocessConfig config);

  core::domain::VarView const& var_view() const noexcept;

  [[nodiscard]] bool preprocess(core::sat::Problem const& problem);

 private:
  PreprocessConfig _config;
  core::domain::VarView _var_view;
};

MAKE_REFS(Preprocess);

}  // namespace ea::preprocess

#endif  // ITMO_PARSAT_PREPROCESS_H
