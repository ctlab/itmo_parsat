#include "Random.h"

namespace {

thread_local ::util::random::Generator* _this_thread_generator = nullptr;

}  // namespace

namespace util::random {

Generator::Generator(uint32_t seed) : _seed(seed), _mt(seed) {
  IPS_VERIFY(
      _this_thread_generator == nullptr &&
      bool("Trying to create second Generator in the same thread."));
  _this_thread_generator = this;
}

Generator::Generator(Generator const& other) : Generator(other._seed) {}

Generator::~Generator() noexcept {
  IPS_VERIFY(
      _this_thread_generator != nullptr && bool("This thread did not initialize Generator."));
  _this_thread_generator = nullptr;
}

Generator& Generator::current_thread_generator() {
  IPS_VERIFY(
      _this_thread_generator != nullptr && bool("This thread did not initialize Generator."));
  return *_this_thread_generator;
}

std::mt19937& stdgen() {
  IPS_VERIFY(
      _this_thread_generator != nullptr && bool("This thread did not initialize Generator."));
  return _this_thread_generator->_mt;
}

bool flip_coin(double p) {
  return p >= std::uniform_real_distribution<double>(0., 1.)(stdgen());
}

bool flip_coin(uint32_t inv) {
  return 1 >= sample<uint32_t>(1, inv);
}

bool flip_coin(uint32_t a, uint32_t b) {
  return a >= sample<uint32_t>(1, b);
}

}  // namespace util::random