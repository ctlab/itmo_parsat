#ifndef ITMO_PARSAT_GENERATE_H
#define ITMO_PARSAT_GENERATE_H

#include <functional>

#include "util/Random.h"
#include "core/types.h"

namespace common {

core::lit_vec_t gen_assumption(int min_size, int max_size, int max_var);

std::vector<int> gen_vars(size_t size, int num_vars);

Mini::vec<Mini::Lit> to_mini(std::vector<int> const& vars);

void pass_assumptions(
    std::function<void(core::lit_vec_t&)> const& f, int min_size,
    int max_size, int max_var, int num_tests);

}  // namespace common

#endif  // ITMO_PARSAT_GENERATE_H
