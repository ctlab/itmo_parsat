#include <glog/logging.h>
#include <gtest/gtest.h>

#include "util/Generator.h"

using namespace core;

void test_flip_coin(double p) {
  double count = 0;
  int samples = 1000000.;
  for (int i = 0; i < samples; ++i) {
    count += random::flip_coin(p);
  }
  ASSERT_LE(std::abs(p - count / samples), 1e-2);
}

TEST(random, flip_coin) {
  core::Generator gen(1337);
  test_flip_coin(0.0);
  test_flip_coin(0.1);
  test_flip_coin(0.5);
  test_flip_coin(0.7);
  test_flip_coin(1.0);
  test_flip_coin(1. / 200.);
}
