#include "core/util/SigHandler.h"

namespace {

core::signal::SigHandler* _sig_handler = nullptr;

}  // namespace

namespace core::signal {

SigHandler::SigHandler() {
  IPS_VERIFY_S(_sig_handler == nullptr, "SigHandler registered more than once.");
  _sig_handler = this;
  sigset_t ss;
  IPS_SYSCALL(sigemptyset(&ss));
  IPS_SYSCALL(sigaddset(&ss, SIGINT));
  IPS_SYSCALL(pthread_sigmask(SIG_BLOCK, &ss, nullptr));

  _t = std::thread([ss, this] {
    timespec ts;
    ts.tv_nsec = 1e8;  // 0.1s
    ts.tv_sec = 0;
    siginfo_t si;

    for (;;) {
      int err = sigtimedwait(&ss, &si, &ts);
      if (_shutdown) {
        break;
      }
      if (err == -1) {
        if (errno == EAGAIN) {
          continue;
        } else {
          IPS_TERMINATE();
        }
      }
      IPS_INFO("SigHandler: caught interrupt.");
      _interrupted = true;
      core::event::raise(event::INTERRUPT);
    }
  });
}

SigHandler::~SigHandler() noexcept {
  if (_t.joinable()) {
    _shutdown = true;
    _t.join();
  }
  _sig_handler = nullptr;
}

bool SigHandler::is_set() {
  return _interrupted;
}

void SigHandler::unset() {
  _interrupted = false;
}

bool is_set() {
  return _sig_handler != nullptr && _sig_handler->is_set();
}

void unset() {
  if (_sig_handler != nullptr) {
    _sig_handler->unset();
  }
}

}  // namespace core::signal
