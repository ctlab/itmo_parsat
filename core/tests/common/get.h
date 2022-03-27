#ifndef ITMO_PARSAT_GET_H
#define ITMO_PARSAT_GET_H

#include "core/tests/common/paths.h"
#include "core/evol/algorithm/Algorithm.h"
#include "core/sat/solver/sequential/Solver.h"
#include "core/sat/solver/service/SolverService.h"
#include "util/CliConfig.h"

namespace common {

enum ElimSetting {
  DO_ELIM,
  NO_ELIM,
  BOTH,
};

void set_logger_config();

core::sat::prop::RProp get_prop(std::filesystem::path const& config_path);

core::sat::solver::RSolver get_solver(std::filesystem::path const& config_path);

core::sat::solver::RSolverService get_solver_service(
    std::filesystem::path const& config_path);

ea::preprocess::RPreprocess get_preprocess();

ea::algorithm::RAlgorithm get_algorithm(
    core::sat::prop::RProp const& rprop,
    std::filesystem::path const& config_path);

void load_problem(
    Minisat::SimpSolver& solver, core::sat::Problem const& problem);

core::sat::Problem get_problem(std::string const& input_name, bool eliminate);

}  // namespace common

#endif  // ITMO_PARSAT_GET_H
