#include <benchmark/benchmark.h>

#include "core/tests/common/paths.h"
#include "core/tests/common/get.h"
#include "core/tests/common/generate.h"

#include "core/search/RandomSearch.h"
#include "core/sat/prop/Prop.h"
#include "util/Random.h"
#include "util/stream.h"

#define BM_PROP_GROUP true, false, "aaai"
#define SAMPLES 5

namespace {

core::sat::prop::RProp get_prop(uint32_t threads) {
  PropConfig prop_config;
  if (threads == 1) {
    prop_config.set_prop_type("SimpProp");
  } else {
    prop_config.set_prop_type("ParProp");
    prop_config.mutable_par_prop_config()->set_max_threads(threads);
    prop_config.mutable_par_prop_config()->set_tree_split_vars(10);
    prop_config.mutable_par_prop_config()->set_seq_split_samples(8192);
  }
  return core::sat::prop::RProp(core::sat::prop::PropRegistry::resolve(prop_config));
}

}  // namespace

static void run_propagate_random(core::sat::prop::Prop& prop, int size, int num_vars, uint64_t total) {
  common::iter_vars(
      [&](core::vars_set_t const& vars) {
        core::search::USearch search = core::search::createRandomSearch(vars, total);
        prop.prop_search(std::move(search));
      },
      size, size, num_vars, SAMPLES);
}

static void run_propagate_full(core::sat::prop::Prop& prop, int size, int num_vars) {
  common::iter_vars(
      [&](core::vars_set_t const& vars) {
        core::search::USearch search = core::search::createFullSearch(vars);
        prop.prop_search(std::move(search));
      },
      size, size, num_vars, SAMPLES);
}

static void run_propagate_tree(core::sat::prop::Prop& prop, int size, int num_vars) {
  common::iter_vars(
      [&](core::vars_set_t const& vars) { prop.prop_tree(common::to_mini(vars), 0); }, size, size, num_vars, SAMPLES);
}

static void BM_prop_random(benchmark::State& state) {
  util::random::Generator generator(239);
  auto problem = common::problems(BM_PROP_GROUP)[state.range(0)];
  auto prop = get_prop(state.range(1));
  prop->load_problem(problem);
  for (auto _ : state) {
    run_propagate_random(*prop, state.range(2), prop->num_vars(), state.range(3));
  }
}

static void BM_prop_full(benchmark::State& state) {
  util::random::Generator generator(239);
  auto problem = common::problems(BM_PROP_GROUP)[state.range(0)];
  auto prop = get_prop(state.range(1));
  prop->load_problem(problem);
  for (auto _ : state) {
    run_propagate_full(*prop, state.range(2), prop->num_vars());
  }
}

static void BM_prop_tree(benchmark::State& state) {
  util::random::Generator generator(239);
  auto problem = common::problems(BM_PROP_GROUP)[state.range(0)];
  auto prop = get_prop(state.range(1));
  prop->load_problem(problem);
  for (auto _ : state) {
    run_propagate_tree(*prop, state.range(2), prop->num_vars());
  }
}

BENCHMARK(BM_prop_random)
    ->ArgsProduct(
        {benchmark::CreateDenseRange(0, (int) common::problems(BM_PROP_GROUP).size() - 1, 1),
         benchmark::CreateRange(1 << 0, 1 << 4, 2),      // threads count
         benchmark::CreateDenseRange(2, 18, 4),          // size of assumption
         benchmark::CreateRange(1 << 13, 1 << 18, 2)});  // number of samples: 2^13 to 2^18

BENCHMARK(BM_prop_full)
    ->ArgsProduct(
        {benchmark::CreateDenseRange(0, (int) common::problems(BM_PROP_GROUP).size() - 1, 1),
         benchmark::CreateRange(1 << 0, 1 << 4, 2),  // threads count
         benchmark::CreateDenseRange(2, 18, 4)});    // size of assumption

BENCHMARK(BM_prop_tree)
    ->ArgsProduct(
        {benchmark::CreateDenseRange(0, (int) common::problems(BM_PROP_GROUP).size() - 1, 1),
         benchmark::CreateRange(1 << 0, 1 << 4, 2),  // threads count
         benchmark::CreateDenseRange(2, 18, 4)});    // size of assumption
