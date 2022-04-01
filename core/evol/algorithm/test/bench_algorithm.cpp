#include <benchmark/benchmark.h>

#include "core/tests/common/paths.h"
#include "core/tests/common/get.h"
#include "core/tests/common/generate.h"

void run_benchmark(
    ea::preprocess::RPreprocess const& preprocess, std::string const& config,
    core::sat::Problem const& problem, core::lit_vec_t const& base_assumption) {
  auto rprop = common::get_prop(common::configs_path + "par_prop.json");
  auto algorithm = common::get_algorithm(rprop, common::configs_path + config);
  rprop->load_problem(problem);
  algorithm->prepare(preprocess);
  algorithm->set_base_assumption(base_assumption);
  algorithm->process();
}

void BM_algorithm(benchmark::State& state, std::string config) {
  common::set_logger_config();
  util::random::Generator gen(239);
  FS_PROBLEM_INPUT(problem, common::inputs("small")[state.range(0)]);
  auto preprocess = common::get_preprocess();
  if (!preprocess->preprocess(problem)) {
    return;
  }
  for (auto _ : state) {
    run_benchmark(preprocess, config, problem, {});
  }
}

BENCHMARK_CAPTURE(BM_algorithm, ea, "ea_prod.json")
    ->DenseRange(0, (int) common::inputs("small").size() - 1, 1);

BENCHMARK_CAPTURE(BM_algorithm, ga, "ga_prod.json")
    ->DenseRange(0, (int) common::inputs("small").size() - 1, 1);
