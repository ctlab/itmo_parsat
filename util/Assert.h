#ifndef ITMO_PARSAT_ASSERT_H
#define ITMO_PARSAT_ASSERT_H

#include <atomic>
#include <iostream>
#include <array>
#include <chrono>
#include <cassert>
#include <sys/syscall.h>
#include <unistd.h>
#include <execinfo.h>
#include <cstdio>
#include <cstdarg>
#include <csignal>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <boost/current_function.hpp>
#include <boost/preprocessor/stringize.hpp>

void print(char const* format, ...);

namespace util::assert::_details {

/**
 * @note Should not be called directly.
 */
void panic(char const* file, long line, char const* function, char const* expr, bool abort, char const* format, ...);

}  // namespace util::assert::_details

#define IPS_UNLIKELY(x) __builtin_expect(x, 0)
#define IPS_LIKELY(x) __builtin_expect(x, 1)

#define IPS_TERMINATE(...)                                                                                    \
  do {                                                                                                        \
    ::util::assert::_details::panic(__FILE__, __LINE__, "teminate_handler()", nullptr, true, "" __VA_ARGS__); \
    __builtin_unreachable();                                                                                  \
  } while (false)

#ifndef IPS_DISABLE_VERIFY

/// @note: Main error handling strategy: immediate failure as there is no sense to solve when failed
#define IPS_VERIFY_S(cond, stream)                                                                       \
  do {                                                                                                   \
    if (IPS_UNLIKELY(!static_cast<bool>(cond))) {                                                        \
      std::cerr << "IPS_VERIFY: failed condition: " #cond << "\nmessage: " << stream << std::endl;       \
      ::util::assert::_details::panic(__FILE__, __LINE__, "IPS_VERIFY] " #cond, nullptr, true, nullptr); \
      __builtin_unreachable();                                                                           \
    }                                                                                                    \
  } while (false)

#define IPS_VERIFY(cond)                                                                                 \
  do {                                                                                                   \
    if (IPS_UNLIKELY(!static_cast<bool>(cond))) {                                                        \
      std::cerr << "IPS_VERIFY: failed condition: " #cond << std::endl;                                  \
      ::util::assert::_details::panic(__FILE__, __LINE__, "IPS_VERIFY] " #cond, nullptr, true, nullptr); \
      __builtin_unreachable();                                                                           \
    }                                                                                                    \
  } while (false)

#define IPS_SYSCALL(call)                                                                                           \
  [&](char const* syscall_function) noexcept {                                                                      \
    auto syscall_result = call;                                                                                     \
    if (IPS_UNLIKELY(syscall_result == -1)) {                                                                       \
      ::util::assert::_details::panic(                                                                              \
          __FILE__, __LINE__, syscall_function, BOOST_PP_STRINGIZE(call), true, "%s (%d)", strerror(errno), errno); \
      __builtin_unreachable();                                                                                      \
    }                                                                                                               \
    return syscall_result;                                                                                          \
  }(BOOST_CURRENT_FUNCTION)

#else

#define IPS_VERIFY_S(...)

#define IPS_VERIFY(...)

#define IPS_SYSCALL(call) call

#endif

#endif  // ITMO_PARSAT_ASSERT_H
