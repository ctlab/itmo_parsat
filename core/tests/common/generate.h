#ifndef ITMO_PARSAT_GENERATE_H
#define ITMO_PARSAT_GENERATE_H

#include <functional>

#include "core/tests/common/paths.h"
#include "util/Random.h"
#include "core/types.h"
#include "core/sat/Problem.h"
#include "core/domain/VarView.h"

namespace common {

core::lit_vec_t gen_assumption(int min_size, int max_size, int max_var);

core::lit_vec_t gen_assumption(
    core::domain::VarView const& var_view, int min_size, int max_size);

std::vector<int> gen_vars(size_t size, int num_vars);

std::vector<int> gen_unique_vars(size_t size, int num_vars);

Mini::vec<Mini::Lit> to_mini(std::vector<int> const& vars);

void iter_assumptions(
    std::function<void(core::lit_vec_t&)> const& f, int min_size, int max_size,
    int max_var, int num_tests);

void iter_assumptions(
    std::function<void(core::lit_vec_t&)> const& f,
    core::domain::VarView const& var_view, int min_size, int max_size,
    int num_tests);

}  // namespace common

#endif  // ITMO_PARSAT_GENERATE_H
