#include <benchmark/benchmark.h>

#include "core/tests/common/paths.h"
#include "core/tests/common/get.h"
#include "core/tests/common/generate.h"

#include "core/domain/assignment/RandomSearch.h"
#include "core/sat/prop/Prop.h"
#include "util/Random.h"

#define BM_PROP_GROUP true, false, "small", "large"

static void run_propagate_naive(
    core::sat::prop::Prop& prop, int size, int num_vars, uint64_t total) {
  common::iter_vars(
      [&](core::vars_set_t const& vars) {
        prop.prop_assignments(core::domain::createRandomSearch(vars, total));
      },
      size, size, num_vars, 10);
}

static void run_propagate_tree(
    core::sat::prop::Prop& prop, int size, int num_vars) {
  common::iter_vars(
      [&](core::vars_set_t const& vars) {
        prop.prop_tree(common::to_mini(vars), 0);
      },
      size, size, num_vars, 10);
}

static void BM_prop_naive(benchmark::State& state, std::string const& config) {
  util::random::Generator generator(239);
  auto problem = common::problems(BM_PROP_GROUP)[state.range(0)];
  auto prop = common::get_prop(common::configs_path + config);
  prop->load_problem(problem);
  for (auto _ : state) {
    run_propagate_naive(
        *prop, state.range(1), prop->num_vars(), state.range(2));
  }
}

static void BM_prop_tree(benchmark::State& state, std::string const& config) {
  util::random::Generator generator(239);
  auto problem = common::problems(BM_PROP_GROUP)[state.range(0)];
  auto prop = common::get_prop(common::configs_path + config);
  prop->load_problem(problem);
  for (auto _ : state) {
    run_propagate_tree(*prop, state.range(1), prop->num_vars());
  }
}

BENCHMARK_CAPTURE(BM_prop_naive, simp, "simp_prop.json")
    ->ArgsProduct(
        {benchmark::CreateDenseRange(
             0, (int) common::problems(BM_PROP_GROUP).size() - 1, 1),
         benchmark::CreateDenseRange(0, 24, 2),
         benchmark::CreateRange(1 << 12, 1 << 16, 2)});

BENCHMARK_CAPTURE(BM_prop_naive, par, "par_prop.json")
    ->ArgsProduct(
        {benchmark::CreateDenseRange(
             0, (int) common::problems(BM_PROP_GROUP).size() - 1, 1),
         benchmark::CreateDenseRange(0, 24, 2),
         benchmark::CreateRange(1 << 12, 1 << 16, 2)});

BENCHMARK_CAPTURE(BM_prop_tree, simp, "simp_prop.json")
    ->ArgsProduct(
        {benchmark::CreateDenseRange(
             0, (int) common::problems(BM_PROP_GROUP).size() - 1, 1),
         benchmark::CreateDenseRange(0, 24, 2)});

BENCHMARK_CAPTURE(BM_prop_tree, par, "par_prop.json")
    ->ArgsProduct(
        {benchmark::CreateDenseRange(
             0, (int) common::problems(BM_PROP_GROUP).size() - 1, 1),
         benchmark::CreateDenseRange(0, 24, 2)});
