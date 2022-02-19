#include "assert.h"
#include <glog/logging.h>

#ifndef __unix__

#define program_invocation_short_name "unknown"

#endif

namespace {

std::array<char, 65536> print_buf_;

void print_(char const* format, va_list args) {
  const size_t size = ::std::min(
      ::vsnprintf(&print_buf_[0], print_buf_.size(), format, args),
      static_cast<int>(print_buf_.size()));
  for (size_t offset = 0; offset < size;) {
    size_t bytes_written = ::write(STDERR_FILENO, &print_buf_[offset], size - offset);
    if (bytes_written <= 0) {
      break;
    }
    offset += bytes_written;
  }
}
}  // namespace

/**
 * This print implementation is equivalent to fprintf(stderr, ...)
 * except it can be called from signal handler (it does no allocations).
 */
void print(char const* format, ...) {
  va_list args;
  va_start(args, format);
  print_(format, args);
  va_end(args);
}

namespace {

// clang-format off
void panic_(
    char const* file,
    long line,
    char const* function,
    char const* expr,
    bool abort,
    char const* format,
    va_list args) {

  thread_local bool this_thread_panics{false};
  static std::atomic_bool run_once{false};
  if (run_once.exchange(true, std::memory_order_relaxed)) {
    if (this_thread_panics) {
      std::signal(SIGABRT, SIG_DFL);
      std::abort();
    }
    for (;;) {
      (void)::pause();
    }
  }
  this_thread_panics = true;

  auto panic_pid = ::getpid();

#if defined(SYS_fork)
  auto pid = ::syscall(SYS_fork);
#else
  auto pid = -1;
#endif

  if (pid == 0 || pid == -1) {
    auto system_usec = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    auto steady_usec = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    constexpr auto USEC_IN_SEC = 1000 * 1000;

    print("\n\n");
    print(
        "Panic at unixtime %ld.%06ld, steady clock time %ld.%06ld, pid = %d.\n",
        system_usec / USEC_IN_SEC, system_usec % USEC_IN_SEC,
        steady_usec / USEC_IN_SEC, steady_usec % USEC_IN_SEC,
        panic_pid);

    if (expr && expr[0] != 0) {
      print(
          "%s: %s:%ld: %s: %s\n",
          program_invocation_short_name, file, line, function, expr
      );
    } else {
      print(
          "%s: %s:%ld: %s\n",
          program_invocation_short_name, file, line, function
      );
    }

    if (format && format[0] != 0) {
      print_(format, args);
    }

    print("\n");
    std::array<void*, 64> stack_buffer;
    auto size = ::backtrace(&stack_buffer[0], stack_buffer.size());
    for (auto i = 0; i < size; ++i) {
      print("%d: ", i);
      ::backtrace_symbols_fd(&stack_buffer[i], 1, STDERR_FILENO);
    }
  }

  if (abort && (pid > 0 || pid == -1)) {
    std::signal(SIGABRT, SIG_DFL);
    LOG(FATAL);
    std::abort();
  }

  if (pid == 0) {
    ::_exit(239);
  }
}
// clang-format on

}  // namespace

namespace core::assert::_details {

// clang-format off
void panic(
    char const* file,
    long line,
    char const* function,
    char const* expr,
    bool abort,
    char const* format,
    ...) {
  va_list args;
  va_start(args, format);
  panic_(file, line, function, expr, abort, format, args);
  va_end(args);
}
// clang-format on

}  // namespace core::assert::_details

[[gnu::unused]] const auto old_terminate_handler = std::set_terminate([] {
  if (const auto exception_ptr = std::current_exception()) {
    try {
      std::rethrow_exception(exception_ptr);
    } catch (std::exception const& e) {
      IPS_TERMINATE("unhandled exception: %s\n", e.what());
    } catch (...) {
      IPS_TERMINATE("unhandled exception: (unknown exception)");
    }
  }
});

bool set_sigabrt_handler() {
  std::signal(SIGABRT, [](int signal) noexcept {
    core::assert::_details::panic(__FILE__, __LINE__, "SIGABRT handler", nullptr, true, nullptr);
  });
  return true;
}

bool set_sigbus_handler() {
  std::signal(SIGBUS, [](int signal) noexcept {
    core::assert::_details::panic(__FILE__, __LINE__, "SIGBUS handler", nullptr, true, nullptr);
  });
  return true;
}

bool set_sigsegv_handler() {
  static std::array<char, MINSIGSTKSZ + 16636> buf;
  stack_t ss;
  ss.ss_sp = &buf[0];
  ss.ss_size = buf.size();
  ss.ss_flags = 0;
  IPS_SYSCALL(::sigaltstack(&ss, nullptr));

  struct sigaction sa {};
  sa.sa_flags = SA_ONSTACK | SA_SIGINFO;
  sa.sa_sigaction = [](int, siginfo_t* info, void* ctx) noexcept {
#ifdef __unix__
    auto context = reinterpret_cast<ucontext_t*>(ctx);
    static constexpr int X86_PF_WRITE = 2;
    bool is_write = context->uc_mcontext.gregs[REG_ERR] & X86_PF_WRITE;
    auto ip = static_cast<std::uintptr_t>(context->uc_mcontext.gregs[REG_RIP]);
    core::assert::_details::panic(
        __FILE__, __LINE__, "SIGSEGV handler", nullptr, false,
        "segmentation fault accessing address 0x%zx; "
        "Instruction at 0x%zx; Access type: %s",
        reinterpret_cast<std::uintptr_t>(info->si_addr), ip, (is_write ? "write" : "read"));
    std::signal(SIGSEGV, SIG_DFL);
#else
    core::assert::_details::panic(
        __FILE__, __LINE__, "SIGSEGV handler", nullptr, false, "segmentation fault");
    std::signal(SIGSEGV, SIG_DFL);
#endif
  };

  IPS_SYSCALL(sigemptyset(&sa.sa_mask));
  IPS_SYSCALL(sigaction(SIGSEGV, &sa, nullptr));
  return true;
}

[[gnu::unused]] const auto sigabrt_handler_is_set = set_sigabrt_handler();

[[gnu::unused]] const auto sigbus_handler_is_set = set_sigbus_handler();

[[gnu::unused]] const auto sigsegv_handler_is_set = set_sigsegv_handler();
