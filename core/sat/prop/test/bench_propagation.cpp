#include <benchmark/benchmark.h>

#include "core/tests/common/paths.h"
#include "core/tests/common/get.h"
#include "core/tests/common/generate.h"

#include "core/search/RandomSearch.h"
#include "core/sat/prop/Prop.h"
#include "util/Random.h"
#include "util/stream.h"

namespace {

util::random::Generator g(239);

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

const core::sat::Problem& get_bvs_7_7_problem() {
  static core::sat::Problem problem(IPS_PROJECT_ROOT "/resources/cnf/aaai/pvs_4_7.cnf");
  //static core::sat::Problem problem(IPS_PROJECT_ROOT "/resources/cnf/aaai/bvs_7_7.cnf");
  return problem;
}

std::vector<std::vector<int>> const& get_backdoors_bvs_7_7() {
  static std::vector<std::vector<int>> backdoors{
      common::gen_vars(18, 1200),
      common::gen_vars(18, 1200),
      common::gen_vars(18, 1200),
      common::gen_vars(18, 1200),
      common::gen_vars(18, 1200),
      //{689, 1120, 939, 754, 1909, 673, 1033, 958, 858, 947},
      //{894, 689, 1120, 939, 754, 1909, 673, 1033, 958, 858, 947},
      //{894, 972, 689, 1120, 1243, 1105, 1579, 755, 819, 771, 1627, 747, 672, 1033, 958, 858, 947},
  };
  return backdoors;
}

}  // namespace

static void run_propagate_random(core::sat::prop::Prop& prop, std::vector<int> const& vars) {
  core::search::USearch search = core::search::createRandomSearch(vars, 1ULL << vars.size());
  prop.prop_search(std::move(search));
}

static void run_propagate_full(core::sat::prop::Prop& prop, std::vector<int> const& vars) {
  core::search::USearch search = core::search::createFullSearch(vars);
  prop.prop_search(std::move(search));
}

static void run_propagate_tree(core::sat::prop::Prop& prop, std::vector<int> const& vars) {
  prop.prop_tree(common::to_mini(vars), 0);
}

static void BM_prop_random(benchmark::State& state) {
  auto const& problem = get_bvs_7_7_problem();
  auto prop = get_prop(state.range(0));
  prop->load_problem(problem);
  auto vars = problem.map_variables(get_backdoors_bvs_7_7()[state.range(1)]);
  for (auto _ : state) {
    run_propagate_random(*prop, vars);
  }
}

static void BM_prop_full(benchmark::State& state) {
  auto const& problem = get_bvs_7_7_problem();
  auto prop = get_prop(state.range(0));
  prop->load_problem(problem);
  auto vars = problem.map_variables(get_backdoors_bvs_7_7()[state.range(1)]);
  for (auto _ : state) {
    run_propagate_full(*prop, vars);
  }
}

static void BM_prop_tree(benchmark::State& state) {
  auto const& problem = get_bvs_7_7_problem();
  auto prop = get_prop(state.range(0));
  prop->load_problem(problem);
  auto vars = problem.map_variables(get_backdoors_bvs_7_7()[state.range(1)]);
  for (auto _ : state) {
    run_propagate_tree(*prop, vars);
  }
}

BENCHMARK(BM_prop_random)
    ->ArgsProduct({
        benchmark::CreateRange(1 << 0, 1 << 3, 1 << 2),
        benchmark::CreateDenseRange(0, (int) get_backdoors_bvs_7_7().size() - 1, 1),
    })
    ->Iterations(1);

BENCHMARK(BM_prop_full)
    ->ArgsProduct({
        benchmark::CreateRange(1 << 0, 1 << 3, 1 << 2),
        benchmark::CreateDenseRange(0, (int) get_backdoors_bvs_7_7().size() - 1, 1),
    })
    ->Iterations(1);

BENCHMARK(BM_prop_tree)
    ->ArgsProduct({
        benchmark::CreateRange(1 << 0, 1 << 3, 1 << 2),
        benchmark::CreateDenseRange(0, (int) get_backdoors_bvs_7_7().size() - 1, 1),
    })
    ->Iterations(1);

