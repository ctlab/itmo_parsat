#ifndef EVOL_SIGHANDLER_H
#define EVOL_SIGHANDLER_H

#include <csignal>
#include <functional>
#include <unordered_map>

namespace core {

class SigHandler {
  friend class CallbackHandle;

 public:
  using callback_t = std::function<void(int)>;

  class CallbackHandle {
   private:
    friend class SigHandler;

   private:
    CallbackHandle(uint64_t handle, SigHandler* handler);

   public:
    void remove();

   private:
    uint64_t handle_ = 0;
    SigHandler* handler_ = nullptr;
  };

 public:
  SigHandler();

  [[nodiscard]] bool is_set() const;

  void unset();

  void set();

  CallbackHandle register_callback(callback_t);

  void callback(int sig);

 private:
  bool registered_{false};
  uint64_t next_handle_{0};
  std::unordered_map<uint64_t, callback_t> callbacks_;
};

}  // namespace core

#endif  // EVOL_SIGHANDLER_H
