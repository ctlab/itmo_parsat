#include "core/util/SigHandler.h"

namespace {

::core::SigHandler* _sig_handler = nullptr;

}  // namespace

namespace core {

SigHandler::CallbackHandle::CallbackHandle(callback_t callback, std::mutex* unlink_mutex)
    : _callback(std::move(callback)), _unlink_mutex(unlink_mutex) {}

SigHandler::CallbackHandle::~CallbackHandle() noexcept {
  remove();
  _unlink_mutex = nullptr;
}

void SigHandler::CallbackHandle::remove() {
  std::lock_guard<std::mutex> lg(*_unlink_mutex);
  unlink();
}

void SigHandler::CallbackHandle::callback(int signal) {
  _callback(signal);
}

SigHandler::SigHandler() {
  IPS_VERIFY(_sig_handler == nullptr && bool("SigHandler re-registered."));
  _sig_handler = this;
  sigset_t ss;
  sigemptyset(&ss);
  sigaddset(&ss, SIGINT);
  pthread_sigmask(SIG_BLOCK, &ss, nullptr);

  /**
   * This thread does all signal handling routine.
   */
  _t = std::thread([ss, this]() {
    timespec ts;
    ts.tv_nsec = 1e8;  // 0.1s
    ts.tv_sec = 0;
    siginfo_t si;

    for (;;) {
      int err = sigtimedwait(&ss, &si, &ts);

      if (_terminate) {
        break;
      }

      if (err == -1 && errno == EAGAIN) {
        continue;
      } else if (err == -1) {
        IPS_TERMINATE();
      }

      IPS_VERIFY(si.si_signo == SIGINT && bool("Only SIGINT is expected in SigHandler"));
      IPS_INFO("Interrupted");
      _registered = true;
      _callback(si.si_signo);
    }
  });
}

SigHandler::~SigHandler() noexcept {
  if (_t.joinable()) {
    _terminate = true;
    _t.join();
  }
  _sig_handler = nullptr;
}

bool SigHandler::is_set() const noexcept {
  return _registered;
}

void SigHandler::unset() noexcept {
  _registered = false;
}

SigHandler::handle_t SigHandler::register_callback(callback_t callback) {
  std::lock_guard<std::mutex> lg(_cb_m);
  auto handle_ptr = std::make_shared<SigHandler::CallbackHandle>(std::move(callback), &_cb_m);
  _handles_list.push_back(*handle_ptr);
  return handle_ptr;
}

void SigHandler::_callback(int signal) {
  if (_handles_list.empty()) {
    // If there are no callbacks registered, terminate is called.
    IPS_TERMINATE("SIGINT");
  }
  for (auto it = _handles_list.begin(); it != _handles_list.end();) {
    (it++)->callback(signal);
  }
}

namespace sig {

SigHandler::handle_t register_callback(SigHandler::callback_t callback) {
  IPS_VERIFY(_sig_handler != nullptr && bool("SigHandler not registered"));
  return _sig_handler->register_callback(std::move(callback));
}

[[nodiscard]] bool is_set() noexcept {
  IPS_VERIFY(_sig_handler != nullptr && bool("SigHandler not registered"));
  return _sig_handler->is_set();
}

void unset() noexcept {
  IPS_VERIFY(_sig_handler != nullptr && bool("SigHandler not registered"));
  _sig_handler->unset();
}

}  // namespace sig
}  // namespace core
