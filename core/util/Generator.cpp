#include "core/util/Generator.h"

namespace {

thread_local ::core::Generator* _this_thread_generator = nullptr;

}  // namespace

namespace core {

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

namespace random {

std::mt19937& stdgen() {
  IPS_VERIFY(
      _this_thread_generator != nullptr && bool("This thread did not initialize Generator."));
  return _this_thread_generator->_mt;
}

bool flip_coin(double p) {
  return p >= std::uniform_real_distribution<double>(0., 1.)(stdgen());
}

}  // namespace random
}  // namespace core