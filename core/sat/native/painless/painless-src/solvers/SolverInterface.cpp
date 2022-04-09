#include "core/sat/native/painless/painless-src/solvers/SolverInterface.h"

#include <atomic>

namespace {
std::atomic_int _id{0};
}  // namespace

namespace painless {

SolverInterface::SolverInterface() : id(_id.fetch_add(1)) {}

}  // namespace painless
