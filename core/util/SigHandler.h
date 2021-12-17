#ifndef EVOL_SIGHANDLER_H
#define EVOL_SIGHANDLER_H

#include <atomic>
#include <mutex>
#include <csignal>
#include <functional>
#include <unordered_map>
#include <thread>

#include "core/util/Logger.h"
#include "core/util/assert.h"

namespace core {

class SigHandler {
  friend class CallbackHandle;

 public:
  using callback_t = std::function<void(int)>;

  class CallbackHandle {
   private:
    friend class SigHandler;

   private:
    CallbackHandle() = default;

   public:
    ~CallbackHandle() noexcept;

    void remove();

   private:
    SigHandler* _handler = nullptr;
  };

 public:
  /**
   * This constructor is expected to be called from the main thread.
   * It blocks SIGINT for the current thread and launches a separate thread
   * that manages SIGINT handling through callbacks.
   */
  SigHandler();

  ~SigHandler() noexcept;

  /**
   * Register callback for SIGINT.
   * Note that callback function will be executed asynchronously in a separate thread,
   * so it must be thread-safe.
   */
  CallbackHandle register_callback(callback_t);

  [[nodiscard]] bool is_set() const;

  void unset();

 private:
  void _callback(int sig);

 private:
  std::mutex _cb_m;
  std::thread _t;
  std::atomic_bool _registered{false};
  std::atomic_bool _terminate{false};
  std::unordered_map<CallbackHandle*, callback_t> _callbacks;
};

}  // namespace core

#endif  // EVOL_SIGHANDLER_H
