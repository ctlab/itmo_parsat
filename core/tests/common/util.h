#ifndef ITMO_PARSAT_UTIL_H
#define ITMO_PARSAT_UTIL_H

#include <gtest/gtest.h>
#include <tuple>

#include "core/tests/common/get.h"

template <typename TupType, size_t... I>
std::ostream& print_tuple(
    std::ostream& os, TupType const& tuple, std::index_sequence<I...>) {
  os << "(";
  (..., (os << (I == 0 ? "" : ", ") << std::get<I>(tuple)));
  return os << ")";
}

template <class... T>
std::ostream& operator<<(std::ostream& os, std::tuple<T...> const& tuple) {
  return print_tuple(os, tuple, std::make_index_sequence<sizeof...(T)>());
}

namespace common {

#define DEFINE_PARAMETRIZED_TEST(NAME, ...)                               \
  class NAME : public ::testing::TestWithParam<std::tuple<__VA_ARGS__>> { \
   protected:                                                             \
    void SetUp() override {                                               \
      std::cerr << std::boolalpha << GetParam() << std::endl;             \
    }                                                                     \
  }

core::sat::State to_state(Minisat::lbool b);

template <typename T>
std::vector<std::tuple<T>> to_tuple(std::vector<T> const& a) {
  std::vector<std::tuple<T>> result;
  result.reserve(a.size());
  for (auto const& t : a) {
    result.push_back(std::make_tuple(t));
  }
  return result;
}

template <typename T>
decltype(auto) cross(std::vector<T> const& a) {
  return a;
}

template <typename U, typename V>
decltype(auto) cross(std::vector<U> const& a, std::vector<V> const& b) {
  std::vector<decltype(std::tuple_cat(a[0], b[0]))> result;
  result.reserve(a.size() * b.size());
  for (size_t i = 0; i < a.size(); ++i) {
    for (size_t j = 0; j < b.size(); ++j) {
      result.push_back(std::tuple_cat(a[i], b[j]));
    }
  }
  return result;
}

template <typename U, typename V, typename... Args>
decltype(auto) cross(U const& a, V const& b, Args const&... args) {
  return cross(cross(a, b), args...);
}

template <typename T>
decltype(auto) extend(std::vector<T> const& a) {
  return a;
}

template <typename U, typename V>
decltype(auto) extend(std::vector<U> const& a, std::vector<V> const& b) {
  std::vector<decltype(std::tuple_cat(a[0], b[0]))> result;
  assert(!b.empty());
  result.reserve(a.size());
  for (size_t i = 0; i < a.size(); ++i) {
    result.push_back(std::tuple_cat(a[i], b[i % b.size()]));
  }
  return result;
}

template <typename U, typename V, typename... Args>
decltype(auto) extend(U const& a, V const& b, Args const&... args) {
  return extend(extend(a, b), args...);
}

template <typename T>
std::vector<T> concat(std::vector<T> a, std::vector<T> const& b) {
  a.insert(a.end(), b.begin(), b.end());
  return a;
}

}  // namespace common

#endif  // ITMO_PARSAT_UTIL_H
