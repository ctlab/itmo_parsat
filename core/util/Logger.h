#ifndef ITMO_PARSAT_LOGGER_H
#define ITMO_PARSAT_LOGGER_H

#include <array>
#include <glog/logging.h>

#include "core/proto/logging_config.pb.h"

namespace core {

class Logger {
 private:
  struct LogEntry {
    LoggingConfig_Entry config;
    std::atomic_uint32_t cur_counter = 0;

   public:
    LogEntry() = default;

    explicit LogEntry(LoggingConfig_Entry config);
  };

 public:
  static constexpr uint32_t MAX_LOG_TYPES = 16;
  static constexpr uint32_t DEFAULT_VERBOSE_LEVEL = 10;
  static constexpr uint32_t DEFAULT_EVERY_N = 1;

 public:
  /**
   * Only ONE instance of this class should be created in the main thread
   */
  Logger(LoggingConfig const& config);

  ~Logger() noexcept;

 public:
  /**
   * Can be used ONLY when an instance of this class exists
   */
  [[nodiscard]] static bool should_log(LogType log_type) noexcept;

 private:
  [[nodiscard]] bool _should_log(LogType log_type) noexcept;

 private:
  std::array<LogEntry, MAX_LOG_TYPES> entries_{};
};

}  // namespace core

#define IPS_LOG(G_LOG_TYPE, LOG_TYPE) LOG_IF(G_LOG_TYPE, ::core::Logger::should_log(::LogType::LOG_TYPE))
#define IPS_LOG_INFO(LOG_TYPE) IPS_LOG(INFO, LOG_TYPE)
#define IPS_LOG_WARNING(LOG_TYPE) IPS_LOG(WARNING, LOG_TYPE)
#define IPS_LOG_ERROR(LOG_TYPE) IPS_LOG(ERROR, LOG_TYPE)

#endif  // ITMO_PARSAT_LOGGER_H
