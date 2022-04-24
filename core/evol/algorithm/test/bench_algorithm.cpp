#include <benchmark/benchmark.h>

#include "core/tests/common/paths.h"
#include "core/tests/common/get.h"
#include "core/tests/common/generate.h"

#define BENCH_ALG_GROUPS true, false, "exp_u700s"
#define SAMPLES 1

static void run_benchmark_one(
    core::sat::prop::RProp rprop, std::string const& config, core::sat::Problem const& problem,
    core::lit_vec_t const& base_assumption) {
  auto preprocess = common::get_preprocess(rprop);
  if (!preprocess->preprocess(problem)) {
    // Skip the meaningless task (zero-time)
    return;
  }
  auto algorithm = common::get_algorithm(rprop, common::configs_path + config);
  algorithm->prepare(preprocess);
  algorithm->set_base_assumption(base_assumption);
  algorithm->process();
}

static void run_benchmark(core::sat::prop::RProp rprop, std::string const& config, core::sat::Problem const& problem) {
  common::iter_assumptions(
      [&](auto const& assumption) { run_benchmark_one(rprop, config, problem, assumption); },  //
      0, 24, rprop->num_vars(), SAMPLES);
}

static void BM_algorithm(benchmark::State& state, std::string config) {
  // Set silent logging config
  common::set_logger_config();
  // Here and in other places benchmarks' generator has a fixed seed,
  // thus everything will be fair for all configurations.
  core::TimeTracer::clean();
  util::random::Generator gen(239);
  auto problem = common::problems(BENCH_ALG_GROUPS)[state.range(0)];
  auto rprop = common::get_prop(common::configs_path + "par_prop.json");
  rprop->load_problem(problem);
  for (auto _ : state) {
    run_benchmark(rprop, config, problem);
  }
  IPS_TRACE_SUMMARY;
}

// BENCHMARK_CAPTURE(BM_algorithm, ea, "ea_prod.json")
//    ->DenseRange(0, (int) common::problems(BENCH_ALG_GROUPS).size() - 1, 1);

BENCHMARK_CAPTURE(BM_algorithm, ea_speed, "ea_bench.json")
    ->DenseRange(0, (int) common::problems(true, false, "aaai").size() - 1, 1)
    ->Iterations(1);

// BENCHMARK_CAPTURE(BM_algorithm, ga, "ga_prod.json")
//    ->DenseRange(0, (int) common::problems(BENCH_ALG_GROUPS).size() - 1, 1);
