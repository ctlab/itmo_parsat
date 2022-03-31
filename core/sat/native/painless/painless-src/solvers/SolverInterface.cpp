#include "core/sat/native/painless/painless-src/solvers/SolverInterface.h"

#include <atomic>

namespace {
std::atomic_int _id{0};
}  // namespace

SolverInterface::SolverInterface() : id(_id.fetch_add(1)) {}