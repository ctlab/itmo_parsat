#include "evol/include/util/SigHandler.h"

#include <glog/logging.h>

namespace {

ea::SigHandler handler_{};

void sig_handler_(int sig) {
  VLOG(2) << "SigHandler caught interrupt.";
  ea::SigHandler::set();
  ea::SigHandler::callback(sig);
}

}  // namespace

namespace ea {

SigHandler::CallbackHandle::CallbackHandle(uint64_t handle, SigHandler* handler)
    : handle_(handle), handler_(handler) {
}

void SigHandler::CallbackHandle::remove() {
  handler_->callbacks_.erase(handle_);
}

SigHandler::SigHandler() {
  std::signal(SIGINT, sig_handler_);
}

bool SigHandler::is_set() {
  return instance().registered_;
}

void SigHandler::unset() {
  instance().registered_ = false;
}

void SigHandler::set() {
  instance().registered_ = true;
}

SigHandler& SigHandler::instance() {
  return handler_;
}

SigHandler::CallbackHandle SigHandler::register_callback(callback_t cb) {
  uint64_t handle = instance().next_handle_++;
  instance().callbacks_[handle] = std::move(cb);
  return {handle, &instance()};
}

void SigHandler::callback(int sig) {
  std::map<uint64_t, callback_t> copy(instance().callbacks_);
  for (auto& it : copy) {
    (it.second)(sig);
  }
}

}  // namespace ea