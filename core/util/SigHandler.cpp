#include "core/util/SigHandler.h"

#include <glog/logging.h>
#include <memory>

namespace {

core::SigHandler* handler_{};

void sig_handler_(int sig) {
  if (handler_) {
    VLOG(2) << "SigHandler caught interrupt.";
    handler_->set();
    handler_->callback(sig);
  }
}

}  // namespace

namespace core {

SigHandler::CallbackHandle::CallbackHandle(uint64_t handle, SigHandler* handler)
    : handle_(handle), handler_(handler) {}

void SigHandler::CallbackHandle::remove() {
  handler_->callbacks_.erase(handle_);
}

SigHandler::SigHandler() {
  handler_ = this;
  std::signal(SIGINT, sig_handler_);
}

bool SigHandler::is_set() const {
  return registered_;
}

void SigHandler::unset() {
  registered_ = false;
}

void SigHandler::set() {
  registered_ = true;
}

SigHandler::CallbackHandle SigHandler::register_callback(callback_t cb) {
  uint64_t handle = next_handle_++;
  callbacks_[handle] = std::move(cb);
  return {handle, this};
}

void SigHandler::callback(int sig) {
  std::unordered_map<uint64_t, callback_t> copy(callbacks_);
  for (auto& it : copy) {
    (it.second)(sig);
  }
}

}  // namespace core