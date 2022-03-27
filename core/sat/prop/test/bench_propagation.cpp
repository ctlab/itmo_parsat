#include <benchmark/benchmark.h>

#include <filesystem>

#include "core/tests/common/paths.h"
#include "core/tests/common/get.h"
#include "core/tests/common/generate.h"

#include "core/sat/prop/SimpProp.h"
#include "core/sat/prop/ParProp.h"
#include "util/Random.h"

// clang-format off
struct prop_assignments_t {} prop_assignments;
struct prop_tree_t {} prop_tree;
struct prop_with_dummy_t {} prop_with_dummy;
// clang-format on

static void run_propagate(
    core::sat::prop::Prop& prop, core::domain::USearch p_search,
    prop_assignments_t) {
  prop.prop_assignments(std::move(p_search), [](auto, auto) { return true; });
}

static void run_propagate(
    core::sat::prop::Prop& prop, core::domain::USearch p_search,
    prop_with_dummy_t) {
  auto& search = *p_search;
  do {
    Mini::vec<Mini::Lit> dummy;
    (bool) prop.propagate(search(), dummy);
  } while (++search);
}

static void run_propagate(
    core::sat::prop::Prop& prop, Minisat::vec<Minisat::Lit> const& vars,
    prop_tree_t) {
  prop.prop_tree(vars, 0);
}

template <typename T, typename TestType, typename VarsF, typename... Args>
void test_search(
    benchmark::State& state, TestType type, VarsF const& vars_f,
    Args&&... args) {
  util::random::Generator generator(239);
  FS_PATH_INPUT(input, common::large_inputs[state.range(1)]);
  T prop(std::forward<Args>(args)...);
  core::sat::Problem problem(input);
  prop.load_problem(problem);
  for (auto _ : state) {
    run_propagate(prop, vars_f(state.range(0), prop.num_vars()), type);
  }
}

template <typename T, typename... Args>
void full_search(benchmark::State& state, Args&&... args) {
  test_search<T>(
      state, prop_assignments,
      [](int size, int nvars) {
        return core::domain::createFullSearch(common::gen_vars(size, nvars));
      },
      std::forward<Args>(args)...);
}

template <typename T, typename... Args>
void full_search_with_dummy(benchmark::State& state, Args&&... args) {
  test_search<T>(
      state, prop_with_dummy,
      [](int size, int nvars) {
        return core::domain::createFullSearch(common::gen_vars(size, nvars));
      },
      std::forward<Args>(args)...);
}

template <typename T, typename... Args>
void tree_search(benchmark::State& state, Args&&... args) {
  test_search<T>(
      state, prop_tree,
      [](int size, int nvars) {
        return common::to_mini(common::gen_vars(size, nvars));
      },
      std::forward<Args>(args)...);
}

static void BM_full_minisat(benchmark::State& state) {
  full_search<core::sat::prop::SimpProp>(state);
}

static void BM_full_minisat_dummy(benchmark::State& state) {
  full_search_with_dummy<core::sat::prop::SimpProp>(state);
}

static void BM_full_par_16_minisat(benchmark::State& state) {
  ParPropConfig config;
  config.set_max_threads(16);
  config.mutable_prop_config()->set_prop_type("SimpProp");
  full_search<core::sat::prop::ParProp>(state, config);
}

static void BM_tree_minisat(benchmark::State& state) {
  tree_search<core::sat::prop::SimpProp>(state);
}

static void BM_tree_par_16_minisat(benchmark::State& state) {
  ParPropConfig config;
  config.set_max_threads(16);
  config.mutable_prop_config()->set_prop_type("SimpProp");
  tree_search<core::sat::prop::ParProp>(state, config);
}

BENCHMARK(BM_full_minisat)
    ->ArgsProduct(
        {benchmark::CreateDenseRange(0, 20, 4),
         benchmark::CreateDenseRange(0, N_INPUTS - 1, 1)});

BENCHMARK(BM_full_minisat_dummy)
    ->ArgsProduct(
        {benchmark::CreateDenseRange(0, 20, 4),
         benchmark::CreateDenseRange(0, N_INPUTS - 1, 1)});

BENCHMARK(BM_full_par_16_minisat)
    ->ArgsProduct(
        {benchmark::CreateDenseRange(0, 20, 4),
         benchmark::CreateDenseRange(0, N_INPUTS - 1, 1)});

BENCHMARK(BM_tree_minisat)
    ->ArgsProduct(
        {benchmark::CreateDenseRange(0, 20, 4),
         benchmark::CreateDenseRange(0, N_INPUTS - 1, 1)});

BENCHMARK(BM_tree_par_16_minisat)
    ->ArgsProduct(
        {benchmark::CreateDenseRange(0, 20, 4),
         benchmark::CreateDenseRange(0, N_INPUTS - 1, 1)});
