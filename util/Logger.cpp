#include "Logger.h"

namespace core {

Logger::Logger() {
  for (size_t i = 0; i < entries_.size(); ++i) {
    entries_[i].config.set_log_type(LogType(i));
    entries_[i].config.set_every_n(DEFAULT_EVERY_N);
    entries_[i].config.set_verbose_level(DEFAULT_VERBOSE_LEVEL);
  }
}

void Logger::_set_logger_config(LoggingConfig const& config) noexcept {
  for (auto const& entry_config : config.entries()) {
    IPS_VERIFY(static_cast<size_t>(entry_config.log_type()) < entries_.size());
    entries_[entry_config.log_type()].config = entry_config;
  }
}

bool Logger::_should_log(LogType log_type) noexcept {
  /* This will not work absolutely correctly in terms of parallel algorithms,
   * but it's ok */
  auto& entry = entries_[log_type];
  bool should;
  if (fLI::FLAGS_v >= (int32_t) entry.config.verbose_level() &&
      entry.cur_counter == 0) {
    should = true;
  } else {
    should = false;
  }
  if (++entry.cur_counter == entry.config.every_n()) {
    entry.cur_counter = 0;
  }
  return should;
}

Logger& Logger::instance() {
  static Logger _logger;
  return _logger;
}

void Logger::set_logger_config(const LoggingConfig& config) {
  instance()._set_logger_config(config);
}

bool Logger::should_log(LogType log_type) noexcept {
  return instance()._should_log(log_type);
}

}  // namespace core
