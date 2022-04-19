#include <gtest/gtest.h>
#include <cmath>

#include "core/tests/common/util.h"
#include "core/tests/common/generate.h"

#include "core/search/RandomSearch.h"
#include "core/search/FullSearch.h"
#include "core/search/CartesianSearch.h"
#include "util/stream.h"
#include "util/Random.h"

namespace {

std::vector<Mini::Lit> gen_mv(size_t size, int s1, int s2) {
  std::vector<Mini::Lit> v(size);
  for (int i = 0; i < v.size(); ++i) {
    v[i] = Mini::mkLit((s1 + 1) * 100 + i, s2 & (1 << i));
  }
  return v;
}

std::tuple<uint64_t, std::vector<std::vector<std::vector<Mini::Lit>>>> gen_cartesian(
    int prod_size, int max_num_assumptions, int max_assumption_size) {
  std::vector<std::vector<std::vector<Mini::Lit>>> csp(prod_size);
  uint64_t csp_size = 1;
  for (size_t i = 0; i < prod_size; ++i) {
    int assumption_size = util::random::sample<int>(0, max_assumption_size);
    int num_assumptions = std::min(1 << assumption_size, util::random::sample<int>(1, max_num_assumptions));
    csp_size *= num_assumptions;
    for (int j = 0; j < num_assumptions; ++j) {
      csp[i].push_back(gen_mv(assumption_size, i, j));
    }
  }
  return {csp_size, csp};
}

}  // namespace

using namespace core;

static constexpr size_t MAX_VAR = 128;

static std::vector<int> to_std_vec(core::lit_vec_t const& vars) {
  std::vector<int> result;
  for (int j = 0; j < vars.size(); ++j) {
    result.push_back(vars[j].x);
  }
  return result;
}

static void test_assignment_full(search::USearch search, uint32_t target_count, uint32_t max_delta) {
  std::set<std::vector<int>> uniques;
  do {
    uniques.insert(to_std_vec((*search)()));
  } while (++(*search));
  int delta = std::abs((int) uniques.size() - (int) target_count);
  ASSERT_LE(delta, max_delta) << "Number of unique items mismatched.";
}

static void test_assignment_split_search(
    search::USearch search, uint32_t target_count, uint32_t max_delta, uint32_t ranges) {
  std::set<std::vector<int>> uniques;
  uint32_t total = 0;
  for (uint32_t index = 0; index < ranges; ++index) {
    auto split = search->split_search(ranges, index);
    do {
      if (split->empty()) { break; }
      ++total;
      uniques.insert(to_std_vec((*split)()));
    } while (++(*split));
  }
  int delta = std::abs((int) uniques.size() - (int) target_count);
  ASSERT_LE(delta, max_delta);
}

void test_full_search(int size) {
  util::random::Generator gen(239);
  search::USearch search(search::createFullSearch(common::gen_unique_vars(size, MAX_VAR)));
  test_assignment_full(std::move(search), (uint32_t) std::pow(2UL, size), 0);
}

void test_random_search(int size, int total) {
  util::random::Generator gen(239);
  bool unique = size <= 63;
  uint32_t max_delta = unique ? 0 : std::max(1, total / 100);
  search::USearch search(search::createAutoRandomSearch(common::gen_unique_vars(size, MAX_VAR), total));
  test_assignment_full(std::move(search), total, max_delta);
}

void test_full_search_split(int size, int num_split) {
  util::random::Generator gen(239);
  search::USearch search(search::createFullSearch(common::gen_unique_vars(size, MAX_VAR)));
  test_assignment_split_search(std::move(search), (uint32_t) std::pow(2UL, size), 0, num_split);
}

void test_random_search_split(long size, int total, int num_split) {
  util::random::Generator gen(239);
  bool unique = size <= 63;
  uint32_t max_delta = unique ? 0 : std::max(1, total / 100);
  search::USearch search(search::createAutoRandomSearch(common::gen_unique_vars(size, MAX_VAR), total));
  test_assignment_split_search(std::move(search), total, max_delta, num_split);
}

void test_cartesian_search(int num_prod) {
  util::random::Generator gen(239);
  int num_tests = 100;
  while (num_tests--) {
    auto [csp_size, csp] = gen_cartesian(num_prod, 10, 10);
    auto search = search::createCartesianSearch(csp);
    test_assignment_full(std::move(search), csp_size, 0);
  }
}

void test_cartesian_search_split(int num_prod, int num_split) {
  util::random::Generator gen(239);
  int num_tests = 100;
  while (num_tests--) {
    auto [csp_size, csp] = gen_cartesian(num_prod, 10, 10);
    auto search = search::createCartesianSearch(csp);
    test_assignment_split_search(search::createCartesianSearch(csp), csp_size, 0, num_split);
  }
}

DEFINE_PARAMETRIZED_TEST(TestFullSearch, int);

TEST_P(TestFullSearch, correctness) {
  auto [size] = GetParam();
  test_full_search(size);
}

INSTANTIATE_TEST_CASE_P(FullSearch, TestFullSearch, ::testing::ValuesIn(common::to_tuple<int>({1, 2, 8, 16})));

DEFINE_PARAMETRIZED_TEST(TestFullSearchSplit, int, int /* num split */);

TEST_P(TestFullSearchSplit, correctness) {
  auto [size, num_split] = GetParam();
  test_full_search_split(size, num_split);
}

INSTANTIATE_TEST_CASE_P(
    FullSearchSplit, TestFullSearchSplit,
    ::testing::ValuesIn(common::cross(common::to_tuple<int>({1, 2, 8, 16}), common::to_tuple<int>({1, 2, 8, 16}))));

DEFINE_PARAMETRIZED_TEST(TestRandomSearch, int, int /* total */);

TEST_P(TestRandomSearch, correctness) {
  auto [size, total] = GetParam();
  test_random_search(size, total);
}

INSTANTIATE_TEST_CASE_P(
    RandomSearch, TestRandomSearch,
    ::testing::ValuesIn(std::vector<std::tuple<int, int>>{
        {1, 2},
        {2, 4},
        {64, 10000},
        {80, 5000},
        {128, 65536},
    }));

DEFINE_PARAMETRIZED_TEST(TestRandomSearchSplit, int, int /* total */, int /* num split*/);

TEST_P(TestRandomSearchSplit, correctness) {
  auto [size, total, num_split] = GetParam();
  test_random_search_split(size, total, num_split);
}

INSTANTIATE_TEST_CASE_P(
    RandomSearchSplit, TestRandomSearchSplit,
    ::testing::ValuesIn(std::vector<std::tuple<int, int, int>>{
        {16, 5000, 2},
        {16, 5000, 8},
        {16, 5000, 16},
    }));

DEFINE_PARAMETRIZED_TEST(TestCartesianSearch, int /* num_prod */);

TEST_P(TestCartesianSearch, correctness) {
  auto [num_prod] = GetParam();
  test_cartesian_search(num_prod);
}

INSTANTIATE_TEST_CASE_P(CartesianSearch, TestCartesianSearch, ::testing::ValuesIn(common::to_tuple<int>({1, 3, 5})));

DEFINE_PARAMETRIZED_TEST(TestCartesianSearchSplit, int /* num_prod */, int /* num_split */);

TEST_P(TestCartesianSearchSplit, correctness) {
  auto [num_prod, num_split] = GetParam();
  test_cartesian_search_split(num_prod, num_split);
}

INSTANTIATE_TEST_CASE_P(
    CartesianSearchSplit, TestCartesianSearchSplit,
    ::testing::ValuesIn(common::cross(common::to_tuple<int>({1, 3, 5}), common::to_tuple<int>({1, 2, 8, 16}))));
