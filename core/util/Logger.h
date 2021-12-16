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

#ifndef DISABLE_LOG

#define IPS_LOG(LOG_TYPE, STREAM) LOG(LOG_TYPE) << STREAM
#define IPS_LOG_T(G_LOG_TYPE, LOG_TYPE, STREAM) \
  LOG_IF(G_LOG_TYPE, ::core::Logger::should_log(::LogType::LOG_TYPE)) << STREAM
#define IPS_LOG_IF(LOG_TYPE, COND, STREAM) LOG_IF(LOG_TYPE, COND) << STREAM

#else

#define IPS_LOG(...)
#define IPS_LOG_T(...)
#define IPS_LOG_IF(...)

#endif

#define IPS_INFO_T(LOG_TYPE, STREAM) IPS_LOG_T(INFO, LOG_TYPE, STREAM)
#define IPS_WARNING_T(LOG_TYPE, STREAM) IPS_LOG_T(WARNING, LOG_TYPE, STREAM)
#define IPS_ERROR_T(LOG_TYPE, STREAM) IPS_LOG_T(ERROR, LOG_TYPE, STREAM)

#define IPS_INFO(STREAM) IPS_LOG(INFO, STREAM)
#define IPS_WARNING(STREAM) IPS_LOG(WARNING, STREAM)
#define IPS_ERROR(STREAM) IPS_LOG(ERROR, STREAM)

#define IPS_INFO_IF(COND, STREAM) IPS_LOG_IF(INFO, COND, STREAM)
#define IPS_WARNING_IF(COND, STREAM) IPS_LOG_IF(WARNING, COND, STREAM)
#define IPS_ERROR_IF(COND, STREAM) IPS_LOG_IF(ERROR, COND, STREAM)

#endif  // ITMO_PARSAT_LOGGER_H
