#ifndef EVOL_LOGGING_H
#define EVOL_LOGGING_H

#include <glog/logging.h>
#include <atomic>

#include "evol/proto/config.pb.h"

namespace ea {

class Logger {
  static constexpr int MAX_ENTRIES = 8;

  struct LogEntry {
    LoggerConfig_LoggerEntry config;
    std::atomic_int cur_counter = 0;

   public:
    LogEntry() = default;

    explicit LogEntry(LoggerConfig_LoggerEntry config);
  };

 private:
  [[nodiscard]] bool _should_log(LogType log_type);

 public:
  explicit Logger(char const* argv0, LoggerConfig const& config);

  [[nodiscard]] static bool should_log(LogType log_type);

 private:
  std::array<LogEntry, MAX_ENTRIES> entries_{};
};

}  // ea

#define EALOG(Type) LOG_IF(INFO, ::ea::Logger::should_log(Type))

#endif  // EVOL_LOGGING_H
