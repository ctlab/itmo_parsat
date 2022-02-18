#ifndef EVOL_SIGHANDLER_H
#define EVOL_SIGHANDLER_H

#include <atomic>
#include <csignal>
#include <thread>

#include "EventHandler.h"
#include "Logger.h"
#include "assert.h"

namespace core::signal {

/**
 * @brief Signal handling routine
 */
class SigHandler {
 public:
  /**
   * @brief SigHandler must be created explicitly to carry all the routine
   * connected with signal masking. Also, its destruction time must be
   * known to programmer, because otherwise it will not stop gracefully.
   */
  SigHandler();

  ~SigHandler() noexcept;

  /**
   * @return If the SIGINT has been caught.
   */
  [[nodiscard]] bool is_set();

  /**
   * @brief Resets the set flag.
   */
  void unset();

 private:
  std::thread _t;
  std::atomic_bool _shutdown{false};
  std::atomic_bool _interrupted{false};
};

[[nodiscard]] bool is_set();

void unset();

}  // namespace core::signal

#endif  // EVOL_SIGHANDLER_H
