#ifndef EVOL_SIGHANDLER_H
#define EVOL_SIGHANDLER_H

#include <csignal>
#include <functional>
#include <map>
#include <set>

namespace ea {

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

  static bool is_set();

  static void unset();

  static void set();

  static CallbackHandle register_callback(callback_t);

  static void callback(int sig);

 private:
  static SigHandler& instance();

 private:
  uint64_t next_handle_{0};
  bool registered_{false};
  std::map<uint64_t, callback_t> callbacks_;
};

}  // namespace ea

#endif  // EVOL_SIGHANDLER_H
