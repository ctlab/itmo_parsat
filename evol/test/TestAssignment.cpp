#include <gtest/gtest.h>
#include <cmath>
#include <glog/logging.h>

#include "evol/include/domain/Assignment.h"

using namespace ea;

class TestAssignment : public ::testing::Test {
 protected:
  void SetUp() override {
    mock_vars.resize(NUM_VARS_MAX);
    for (int i = 0; i < NUM_VARS_MAX; ++i) {
      mock_var_map[i] = i;
    }
  }

  void set_size(long size) {
    std::fill(mock_vars.begin(), mock_vars.begin() + size, true);
    std::fill(mock_vars.begin() + size, mock_vars.end(), false);
  }

  static std::vector<int> get_vars(domain::UAssignment const& asgn) {
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
      domain::UAssignment assignment_p, uint32_t target_count, uint32_t max_delta) {
    std::set<std::vector<int>> uniques;
    do {
      auto vars = get_vars(assignment_p);
      uniques.insert(get_vars(assignment_p));
    } while (++(*assignment_p));
    int delta = std::abs((int) uniques.size() - (int) target_count);
    ASSERT_LE(delta, max_delta);
    VLOG(4) << "Delta: " << delta << " when max delta is: " << max_delta;
  }

  static void test_assignment_ranges(
      domain::UAssignment assignment_p, uint32_t target_count, uint32_t max_delta, uint32_t ranges) {
//    std::set<std::vector<int>> uniques;
//    auto& assignment = *assignment_p;
//    uint64_t r_len = target_count / ranges;
//    uint64_t a_num = target_count % ranges;
//    uint64_t start = 0;
//    for (size_t r = 0; r < ranges; ++r) {
//      uint64_t len = r_len + (r < a_num);
//      domain::UAssignment as(assignment_p->clone());
//      as->set_range(start, start + len - 1);
//      CHECK_EQ(as->size(), len);
//      start += len;
//      for (size_t i = 0; i < len; ++i, ++(*as)) {
//        std::vector<int> set_vars;
//        auto const& asgn = (*as)();
//        for (int j = 0; j < asgn.size(); ++j) {
//          if (Minisat::sign(asgn[j])) {
//            set_vars.push_back(j);
//          }
//        }
//        uniques.insert(set_vars);
//      }
//    }
//
//    ASSERT_LE(std::abs((int) uniques.size() - (int) target_count), max_delta);
  }

  void test_full_search(long size) {
    set_size(size);
    domain::UAssignment assignment_p(domain::createFullSearch(mock_var_map, mock_vars));
    test_assignment_full(std::move(assignment_p), (uint32_t) std::pow(2UL, size), 0);
  }

  void test_random_assignments(long size, long target) {
    set_size(size);
    bool unique = size <= 63;
    uint32_t max_delta = unique ? 0 : std::max(1L, target / 100);
    domain::UAssignment assignment_p(domain::createRandomSearch(mock_var_map, mock_vars, target));
    test_assignment_full(std::move(assignment_p), target, max_delta);
  }

 public:
  static constexpr long NUM_VARS_MAX = 128;
  std::map<int, int> mock_var_map;
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

TEST_F(TestAssignment, random_assignments_64_soft) {
  test_random_assignments(64, 10000);
}

TEST_F(TestAssignment, random_assignments_80_soft) {
  test_random_assignments(80, 5000);
}

TEST_F(TestAssignment, random_assignments_128_soft) {
  test_random_assignments(128, 65536);
}

TEST_F(TestAssignment, random_assignments_1_hard) {
  test_random_assignments(1, 2);
}

TEST_F(TestAssignment, random_assignments_2_hard) {
  test_random_assignments(2, 4);
}

TEST_F(TestAssignment, random_assignments_16_hard) {
  test_random_assignments(16, 65536);
}

//TEST_F(TestAssignment, full_search_range_16_2) {
//  set_size(16);
//  domain::UAssignment assignment_p(domain::createFullSearch(mock_var_map, mock_vars));
//  test_assignment_ranges(std::move(assignment_p), (uint32_t) std::pow(2UL, 16), 0, 2);
//}
//
//TEST_F(TestAssignment, full_search_range_16_8) {
//  set_size(16);
//  domain::UAssignment assignment_p(domain::createFullSearch(mock_var_map, mock_vars));
//  test_assignment_ranges(std::move(assignment_p), (uint32_t) std::pow(2UL, 16), 0, 8);
//}
//
//TEST_F(TestAssignment, full_search_range_16_16) {
//  set_size(16);
//  domain::UAssignment assignment_p(domain::createFullSearch(mock_var_map, mock_vars));
//  test_assignment_ranges(std::move(assignment_p), (uint32_t) std::pow(2UL, 16), 0, 16);
//}
//
//TEST_F(TestAssignment, random_range_16_2) {
//  set_size(16);
//  domain::UAssignment assignment_p(domain::createRandomSearch(mock_var_map, mock_vars, 0));
//  test_assignment_ranges(std::move(assignment_p), (uint32_t) std::pow(2UL, 16), 5, 2);
//}
//
//TEST_F(TestAssignment, random_range_16_8) {
//  set_size(16);
//  domain::UAssignment assignment_p(domain::createRandomSearch(mock_var_map, mock_vars, 0));
//  test_assignment_ranges(std::move(assignment_p), (uint32_t) std::pow(2UL, 16), 10, 8);
//}
//
//TEST_F(TestAssignment, random_range_16_16) {
//  set_size(16);
//  domain::UAssignment assignment_p(domain::createRandomSearch(mock_var_map, mock_vars, 0));
//  test_assignment_ranges(std::move(assignment_p), (uint32_t) std::pow(2UL, 16), 20, 16);
//}
