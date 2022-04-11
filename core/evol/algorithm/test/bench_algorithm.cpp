#include <benchmark/benchmark.h>

#include "core/tests/common/paths.h"
#include "core/tests/common/get.h"
#include "core/tests/common/generate.h"

#define BENCH_ALG_GROUPS true, false, "small", "large"
#define SAMPLES 10

static void run_benchmark_one(
    ea::preprocess::RPreprocess const& preprocess, std::string const& config,
    core::sat::Problem const& problem, core::lit_vec_t const& base_assumption) {
  auto rprop = common::get_prop(common::configs_path + "par_prop.json");
  auto algorithm = common::get_algorithm(rprop, common::configs_path + config);
  rprop->load_problem(problem);
  algorithm->prepare(preprocess);
  algorithm->set_base_assumption(base_assumption);
  algorithm->process();
}

static void run_benchmark(
    ea::preprocess::RPreprocess const& preprocess, std::string const& config,
    core::sat::Problem const& problem) {
  common::iter_assumptions(
      [&](auto const& assumption) {
        run_benchmark_one(preprocess, config, problem, assumption);
      },
      0, 20, preprocess->num_vars(), SAMPLES);
}

static void BM_algorithm(benchmark::State& state, std::string config) {
  common::set_logger_config();
  // Here and in other places benchmarks' generator has a fixed seed,
  // thus everything will be fair for all configurations.
  util::random::Generator gen(239);
  auto problem = common::problems(BENCH_ALG_GROUPS)[state.range(0)];
  auto preprocess = common::get_preprocess();
  if (!preprocess->preprocess(problem)) {
    // Skip if the task is meaningless (same for all testing configurations).
    return;
  }
  for (auto _ : state) {
    run_benchmark(preprocess, config, problem);
  }
}

BENCHMARK_CAPTURE(BM_algorithm, ea, "ea_prod.json")
    ->DenseRange(0, (int) common::problems(BENCH_ALG_GROUPS).size() - 1, 1);

BENCHMARK_CAPTURE(BM_algorithm, ga, "ga_prod.json")
    ->DenseRange(0, (int) common::problems(BENCH_ALG_GROUPS).size() - 1, 1);
