#include <benchmark/benchmark.h>

#include "core/tests/common/paths.h"
#include "core/tests/common/get.h"
#include "core/tests/common/generate.h"

#include "core/search/RandomSearch.h"
#include "core/sat/prop/Prop.h"
#include "util/Random.h"
#include "util/stream.h"

#define BM_PROP_GROUP true, false, "small", "large"
#define SAMPLES 10

namespace {

core::sat::prop::RProp get_prop(uint32_t threads) {
  PropConfig prop_config;
  if (threads == 1) {
    prop_config.set_prop_type("SimpProp");
  } else {
    prop_config.set_prop_type("ParProp");
    prop_config.mutable_par_prop_config()->set_max_threads(threads);
    prop_config.mutable_par_prop_config()->set_tree_split_vars(12);
    prop_config.mutable_par_prop_config()->set_seq_split_samples(16384);
  }
  return core::sat::prop::RProp(core::sat::prop::PropRegistry::resolve(prop_config));
}

}  // namespace

static void run_propagate_naive(core::sat::prop::Prop& prop, int size, int num_vars, uint64_t total) {
  common::iter_vars(
      [&](core::vars_set_t const& vars) {
        core::search::USearch search = core::search::createRandomSearch(vars, total);
        prop.prop_search(std::move(search));
      },
      size, size, num_vars, SAMPLES);
}

static void run_propagate_tree(core::sat::prop::Prop& prop, int size, int num_vars) {
  common::iter_vars(
      [&](core::vars_set_t const& vars) { prop.prop_tree(common::to_mini(vars), 0); }, size, size, num_vars, SAMPLES);
}

static void BM_prop_naive(benchmark::State& state) {
  util::random::Generator generator(239);
  auto problem = common::problems(BM_PROP_GROUP)[state.range(0)];
  auto prop = get_prop(state.range(1));
  prop->load_problem(problem);
  for (auto _ : state) {
    run_propagate_naive(*prop, state.range(2), prop->num_vars(), state.range(3));
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

BENCHMARK(BM_prop_naive)
    ->ArgsProduct(
        {benchmark::CreateDenseRange(0, (int) common::problems(BM_PROP_GROUP).size() - 1, 1),
         benchmark::CreateRange(1 << 0, 1 << 4, 2),      // threads count
         benchmark::CreateDenseRange(0, 20, 4),          // size of assumption
         benchmark::CreateRange(1 << 12, 1 << 18, 2)});  // number of samples

BENCHMARK(BM_prop_tree)
    ->ArgsProduct(
        {benchmark::CreateDenseRange(0, (int) common::problems(BM_PROP_GROUP).size() - 1, 1),
         benchmark::CreateRange(1 << 0, 1 << 4, 2),  // threads count
         benchmark::CreateDenseRange(0, 20, 4)});    // size of assumption