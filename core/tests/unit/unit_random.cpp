#include <glog/logging.h>
#include <gtest/gtest.h>

#include "util/Random.h"

using namespace core;

void test_flip_coin(uint32_t a, uint32_t b) {
  double p = (double) a / (double) b;
  double count = 0;
  int samples = 1000000;
  for (int i = 0; i < samples; ++i) {
    count += util::random::flip_coin(a, b);
  }
  ASSERT_LE(std::abs(p - count / samples), 1e-2);
}

TEST(random, flip_coin) {
  util::random::Generator gen(1337);
  test_flip_coin(1, 1);
  test_flip_coin(1, 2);
  test_flip_coin(1, 8);
  test_flip_coin(1, 16);
  test_flip_coin(1, 256);
  test_flip_coin(5, 17);
  test_flip_coin(17, 239);
}
