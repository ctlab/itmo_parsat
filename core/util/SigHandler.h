#ifndef EVOL_SIGHANDLER_H
#define EVOL_SIGHANDLER_H

#include <atomic>
#include <mutex>
#include <memory>
#include <csignal>
#include <functional>
#include <unordered_map>
#include <thread>
#include <boost/intrusive/list.hpp>

#include "core/util/Logger.h"
#include "core/util/assert.h"

namespace core {

/**
 * SIGINT handling routine
 */
class SigHandler {
  friend class CallbackHandle;

 public:
  using callback_t = std::function<void(int)>;

  /**
   * The callback handle returned as a result of registering a callback.
   * Its purpose is to remove the callback by demand or when destructed.
   */
  class CallbackHandle : public boost::intrusive::list_base_hook<
                             boost::intrusive::link_mode<boost::intrusive::auto_unlink>> {
   private:
    friend class SigHandler;

   public:
    CallbackHandle(callback_t callback, std::mutex* unlink_mutex);

   public:
    ~CallbackHandle() noexcept;

    /**
     * Removes the callback from handler.
     */
    void remove();

    /**
     * Executes callback.
     * @param signal the caught signal
     */
    void callback(int signal);

    bool operator<(CallbackHandle const& other) const noexcept;

   private:
    uint32_t _id;
    callback_t _callback;
    std::mutex* _unlink_mutex;
  };

 public:
  typedef std::shared_ptr<CallbackHandle> handle_t;

 public:
  /**
   * SigHandler must be created explicitly to carry all the routine
   * connected with signal masking. Also, its destruction time must be
   * known to programmer, because otherwise it will not stop gracefully.
   */
  SigHandler();

  ~SigHandler() noexcept;

  /**
   * Register callback for SIGINT.
   * Note that callback function will be executed asynchronously in a separate thread,
   * so it must be thread-safe.
   *
   * If there are no registered callbacks, the program is terminated.
   */
  handle_t register_callback(callback_t callback);

  /**
   * Checks whether the handler caught SIGINT yet.
   */
  [[nodiscard]] bool is_set() const noexcept;

  /**
   * Resets set flag.
   */
  void unset() noexcept;

 private:
  void _callback(int signal);

 private:
  std::mutex _cb_m;
  std::thread _t;
  std::atomic_bool _registered{false};
  std::atomic_bool _terminate{false};
  boost::intrusive::list<CallbackHandle, boost::intrusive::constant_time_size<false>> _handles_list;
};

}  // namespace core

#endif  // EVOL_SIGHANDLER_H
