#include <gtest/gtest.h>
#include <cmath>

#include "core/domain/assignment/RandomSearch.h"
#include "core/domain/assignment/FullSearch.h"
#include "core/domain/assignment/CartesianSearch.h"
#include "util/stream.h"
#include "util/Logger.h"
#include "util/Generator.h"

namespace {

std::vector<Minisat::Lit> gen_mv(size_t size, int s1, int s2) {
  std::vector<Minisat::Lit> v(size);
  for (int i = 0; i < v.size(); ++i) {
    v[i] = Minisat::mkLit((s1 + 1) * 100 + i, s2 & (1 << i));
  }
  return v;
}

}  // namespace

using namespace core;

class TestAssignment : public ::testing::Test {
 protected:
  void SetUp() override {
    mock_vars.resize(NUM_VARS_MAX);
    for (int i = 0; i < NUM_VARS_MAX; ++i) {
      mock_var_map.map_var(i, i);
    }
  }

  void set_size(long size) {
    std::fill(mock_vars.begin(), mock_vars.begin() + size, true);
    std::fill(mock_vars.begin() + size, mock_vars.end(), false);
  }

  static std::vector<int> get_vars(Minisat::vec<Minisat::Lit> const& vars) {
    std::vector<int> set_vars;
    for (int j = 0; j < vars.size(); ++j) {
      if (Minisat::sign(vars[j])) {
        set_vars.push_back(j);
      }
    }
    return set_vars;
  }

  static void test_assignment_full(
      domain::USearch assignment_p, uint32_t target_count, uint32_t max_delta) {
    std::set<std::vector<int>> uniques;
    do {
      uniques.insert(get_vars((*assignment_p)()));
    } while (++(*assignment_p));
    int delta = std::abs((int) uniques.size() - (int) target_count);
    IPS_INFO("Uniques: " << uniques.size() << ", when expected: " << target_count);
    ASSERT_LE(delta, max_delta);
  }

  static void test_assignment_split_search(
      domain::USearch assignment_p, uint32_t target_count, uint32_t max_delta, uint32_t ranges) {
    std::set<std::vector<int>> uniques;
    uint32_t total = 0;
    for (uint32_t index = 0; index < ranges; ++index) {
      auto split = assignment_p->split_search(ranges, index);
      do {
        if (split->empty()) {
          break;
        }
        ++total;
        uniques.insert(get_vars((*split)()));
      } while (++(*split));
    }
    int delta = std::abs((int) uniques.size() - (int) target_count);
    IPS_INFO(
        "Uniques: " << uniques.size() << ", when expected: " << target_count
                    << ", total steps: " << total);
    ASSERT_LE(delta, max_delta);
  }

  void test_full_search_range(long size, uint32_t ranges) {
    set_size(size);
    domain::USearch assignment_p(domain::createFullSearch(mock_var_map, mock_vars));
    test_assignment_split_search(
        std::move(assignment_p), (uint32_t) std::pow(2UL, size), 0, ranges);
  }

  void test_random_search_range(long size, uint32_t total, uint32_t ranges) {
    set_size(size);
    domain::USearch assignment_p(domain::createRandomSearch(mock_var_map, mock_vars, total));
    test_assignment_split_search(std::move(assignment_p), total, 0, ranges);
  }

  void test_full_search(long size) {
    set_size(size);
    domain::USearch assignment_p(domain::createFullSearch(mock_var_map, mock_vars));
    test_assignment_full(std::move(assignment_p), (uint32_t) std::pow(2UL, size), 0);
  }

  void test_random_search(long size, long target) {
    set_size(size);
    bool unique = size <= 63;
    uint32_t max_delta = unique ? 0 : std::max(1L, target / 100);
    domain::USearch assignment_p(domain::createRandomSearch(mock_var_map, mock_vars, target));
    test_assignment_full(std::move(assignment_p), target, max_delta);
  }

 public:
  static constexpr long NUM_VARS_MAX = 128;
  core::Generator gen{1337};
  core::domain::VarView mock_var_map;
  std::vector<bool> mock_vars;
};

TEST_F(TestAssignment, full_search_1) {
  test_full_search(1);
}

TEST_F(TestAssignment, full_search_2) {
  test_full_search(2);
}

TEST_F(TestAssignment, full_search_16) {
  test_full_search(16);
}

TEST_F(TestAssignment, random_search_64_soft) {
  test_random_search(64, 10000);
}

TEST_F(TestAssignment, random_search_80_soft) {
  test_random_search(80, 5000);
}

TEST_F(TestAssignment, random_search_128_soft) {
  test_random_search(128, 65536);
}

TEST_F(TestAssignment, random_search_1_hard) {
  test_random_search(1, 2);
}

TEST_F(TestAssignment, random_search_2_hard) {
  test_random_search(2, 4);
}

TEST_F(TestAssignment, random_search_16_hard) {
  test_random_search(16, 65536);
}

TEST_F(TestAssignment, full_search_range_1_2) {
  test_full_search_range(1, 2);
}

TEST_F(TestAssignment, full_search_range_1_16) {
  test_full_search_range(1, 16);
}

TEST_F(TestAssignment, full_search_range_4_2) {
  test_full_search_range(4, 2);
}

TEST_F(TestAssignment, full_search_range_16_2) {
  test_full_search_range(16, 2);
}

TEST_F(TestAssignment, full_search_range_16_8) {
  test_full_search_range(16, 8);
}

TEST_F(TestAssignment, full_search_range_16_16) {
  test_full_search_range(16, 16);
}

TEST_F(TestAssignment, random_search_range_16_2) {
  test_random_search_range(16, 5000, 2);
}

TEST_F(TestAssignment, random_search_range_16_8) {
  test_random_search_range(16, 5000, 8);
}

TEST_F(TestAssignment, random_search_range_16_16) {
  test_random_search_range(16, 5000, 16);
}

TEST_F(TestAssignment, cartesian_search) {
  int num_tests = 100;
  while (num_tests--) {
    std::vector<std::vector<std::vector<Minisat::Lit>>> csp(5);
    uint64_t csp_size = 1;
    for (size_t i = 0; i < 5; ++i) {
      // generate assumption set
      int assump_size = core::random::sample<int>(0, 10);
      int set_size = std::min(1 << assump_size, core::random::sample<int>(1, 10));
      csp_size *= set_size;
      for (int j = 0; j < set_size; ++j) {
        csp[i].push_back(gen_mv(assump_size, i, j));
      }
    }
    auto search = domain::createCartesianSearch(csp);
    std::set<std::vector<int>> assumptions;
    auto& search_ref = *search;
    do {
      auto vars = get_vars(search_ref());
      ASSERT_EQ(0, assumptions.count(vars));
      assumptions.insert(vars);
    } while (++search_ref);
    CHECK_EQ(assumptions.size(), csp_size);
  }
}

TEST_F(TestAssignment, cartesian_search_split) {
  int num_tests = 1000;
  while (num_tests--) {
    int size = core::random::sample(1, 5);
    std::vector<std::vector<std::vector<Minisat::Lit>>> csp(size);
    uint64_t csp_size = 1;
    for (size_t i = 0; i < size; ++i) {
      // generate assumption set
      int assump_size = core::random::sample<int>(0, 10);
      int set_size = std::min(1 << assump_size, core::random::sample<int>(1, 10));
      csp_size *= set_size;
      for (int j = 0; j < set_size; ++j) {
        csp[i].push_back(gen_mv(assump_size, i, j));
      }
    }
    auto search = domain::createCartesianSearch(csp);
    std::set<std::vector<int>> assumptions;

    for (int ns = 0; ns < 16; ++ns) {
      auto split = search->split_search(16, ns);
      auto& split_ref = *split;
      if (split_ref.empty()) {
        break;
      }
      do {
        auto vars = get_vars(split_ref());
        ASSERT_EQ(0, assumptions.count(vars));
        assumptions.insert(vars);
      } while (++split_ref);
    }

    CHECK_EQ(assumptions.size(), csp_size);
  }
}
