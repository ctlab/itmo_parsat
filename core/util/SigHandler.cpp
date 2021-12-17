#include "core/util/SigHandler.h"

#include <memory>

namespace core {

SigHandler::CallbackHandle::~CallbackHandle() noexcept {
  remove();
}

void SigHandler::CallbackHandle::remove() {
  std::lock_guard<std::mutex> lg(_handler->_cb_m);
  _handler->_callbacks.erase(this);
}

SigHandler::SigHandler() {
  sigset_t ss;
  sigemptyset(&ss);
  sigaddset(&ss, SIGINT);
  pthread_sigmask(SIG_BLOCK, &ss, nullptr);

  /**
   * This thread does all signal handling routine.
   */
  _t = std::thread([ss, this]() {
    timespec ts;
    ts.tv_nsec = 1e8; // 0.1s
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
}

bool SigHandler::is_set() const {
  return _registered;
}

void SigHandler::unset() {
  _registered = false;
}

SigHandler::CallbackHandle SigHandler::register_callback(callback_t cb) {
  std::lock_guard<std::mutex> lg(_cb_m);
  CallbackHandle handle;
  handle._handler = this;
  _callbacks[&handle] = std::move(cb);
  return handle;
}

void SigHandler::_callback(int sig) {
  std::unordered_map<CallbackHandle*, callback_t> copy;
  {
    // copy all existing callbacks and release lock
    std::lock_guard<std::mutex> lg(_cb_m);
    copy = _callbacks;
  }
  if (copy.empty()) {
    // If there are no callbacks registered, the terminate handler will be called.
    IPS_TERMINATE("SIGINT");
  }
  for (auto& it : copy) {
    (it.second)(sig);
  }
}

}  // namespace core
