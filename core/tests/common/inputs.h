#ifndef ITMO_PARSAT_INPUTS_H
#define ITMO_PARSAT_INPUTS_H

#include "util/Random.h"
#include "core/sat/native/mini/mtl/Vec.h"
#include "core/sat/native/mini/utils/Lit.h"

namespace common {

#define MAX_INPUT_LEN 128
#define N_INPUTS (sizeof(common::inputs) / MAX_INPUT_LEN)
#define FS_PATH_INPUT(INPUT)   \
  std::filesystem::path INPUT( \
      std::string(IPS_PROJECT_ROOT "/resources/cnf/common/") + common::inputs[state.range(1)])

extern char const inputs[1][MAX_INPUT_LEN];

std::vector<int> gen_vars(size_t size, int num_vars);

Mini::vec<Mini::Lit> to_mini(std::vector<int> const& vars);

}  // namespace common

#endif  // ITMO_PARSAT_INPUTS_H
