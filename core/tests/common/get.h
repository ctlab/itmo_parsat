#ifndef ITMO_PARSAT_GET_H
#define ITMO_PARSAT_GET_H

#include "core/evol/algorithm/Algorithm.h"
#include "core/sat/solver/sequential/Solver.h"
#include "util/CliConfig.h"
#include "core/tests/common/paths.h"

namespace common {

void set_logger_config();

core::sat::prop::RProp get_prop(std::filesystem::path const& config_path);

core::sat::solver::RSolver get_solver(std::filesystem::path const& config_path);

ea::preprocess::RPreprocess get_preprocess();

ea::algorithm::RAlgorithm get_algorithm(
    core::sat::prop::RProp const& rprop,
    std::filesystem::path const& config_path);

void load_problem(
    Minisat::SimpSolver& solver, core::sat::Problem const& problem);

}  // namespace common

#endif  // ITMO_PARSAT_GET_H
