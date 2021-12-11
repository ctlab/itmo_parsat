#include "evol/include/util/Logging.h"

#include <utility>

namespace {

::ea::Logger* logger_;

}  // namespace

namespace ea {

Logger::LogEntry::LogEntry(LoggerConfig_LoggerEntry config) : config(std::move(config)) {}

Logger::Logger(char const* argv0, LoggerConfig const& config) {
  for (size_t i = 0; i < entries_.size(); ++i) {
    entries_[i].config.set_log_type(LogType(i));
    entries_[i].config.set_every_n(1);
    entries_[i].config.set_min_verbose_level(10);
  }

  for (auto const& entry_config : config.entries()) {
    entries_[entry_config.log_type()].config = entry_config;
  }

  CHECK_EQ(logger_, nullptr) << "Logger is initialized yet.";
  logger_ = this;
}

bool Logger::_should_log(LogType log_type) {
  auto& entry = entries_[log_type];
  bool should;
  if (fLI::FLAGS_v >= (int32_t) entry.config.min_verbose_level() && entry.cur_counter == 0) {
    should = true;
  } else {
    should = false;
  }
  if (++entry.cur_counter == entry.config.every_n()) {
    entry.cur_counter = 0;
  }
  return should;
}

bool Logger::should_log(LogType log_type) {
  LOG_IF(FATAL, logger_ == nullptr) << "Logger is used before being initialized.";
  return logger_->_should_log(log_type);
}

}  // namespace ea