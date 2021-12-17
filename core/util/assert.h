#ifndef ITMO_PARSAT_ASSERT_H
#define ITMO_PARSAT_ASSERT_H

#include <atomic>
#include <array>
#include <chrono>
#include <cassert>
#include <sys/syscall.h>
#include <unistd.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdarg.h>
#include <csignal>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <boost/current_function.hpp>
#include <boost/preprocessor/stringize.hpp>

namespace core::assert::_details {

void panic(
    char const* file, long line, char const* function, char const* expr, bool abort,
    char const* format, ...);

}  // namespace core::assert::_details

#define IPS_UNLIKELY(x) __builtin_expect(x, 0)
#define IPS_LIKELY(x) __builtin_expect(x, 1)

#define IPS_VERIFY assert

#define IPS_SYSCALL(call)                                                                  \
  [&](char const* syscall_function) noexcept {                                             \
    auto syscall_result = call;                                                            \
    if (IPS_UNLIKELY(syscall_result == -1)) {                                              \
      ::core::assert::_details::panic(                                                     \
          __FILE__, __LINE__, syscall_function, BOOST_PP_STRINGIZE(call), true, "%s (%d)", \
          strerror(errno), errno);                                                         \
      __builtin_unreachable();                                                             \
    }                                                                                      \
    return syscall_result;                                                                 \
  }(BOOST_CURRENT_FUNCTION)

#define IPS_TERMINATE(...)                                                        \
  do {                                                                            \
    ::core::assert::_details::panic(                                              \
        __FILE__, __LINE__, "teminate_handler()", nullptr, true, "" __VA_ARGS__); \
    __builtin_unreachable();                                                      \
  } while (false)

#endif  // ITMO_PARSAT_ASSERT_H
