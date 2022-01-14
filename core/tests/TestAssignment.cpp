#include <gtest/gtest.h>
#include <cmath>

#include "core/domain/assignment/RandomSearch.h"
#include "core/domain/assignment/FullSearch.h"
#include "core/util/stream.h"
#include "core/util/Logger.h"
#include "core/util/Generator.h"

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

  static std::vector<int> get_vars(domain::USearch const& asgn) {
    std::vector<int> set_vars;
    auto const& vars = (*asgn)();
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
      uniques.insert(get_vars(assignment_p));
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
        uniques.insert(get_vars(split));
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
