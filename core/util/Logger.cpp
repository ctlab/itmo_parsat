#include "core/util/Logger.h"

namespace {

core::Logger* _logger = nullptr;

}  // namespace

namespace core {

Logger::Logger(const LoggingConfig& config) {
  for (size_t i = 0; i < entries_.size(); ++i) {
    entries_[i].config.set_log_type(LogType(i));
    entries_[i].config.set_every_n(DEFAULT_EVERY_N);
    entries_[i].config.set_verbose_level(DEFAULT_VERBOSE_LEVEL);
  }

  for (auto const& entry_config : config.entries()) {
    entries_[entry_config.log_type()].config = entry_config;
  }

  CHECK_EQ(nullptr, _logger) << "Logger is initialized more than once";
  _logger = this;
}

Logger::~Logger() noexcept {
  _logger = nullptr;
}

bool Logger::should_log(LogType log_type) noexcept {
  CHECK_NE(nullptr, _logger) << "Logger is not initialized when calling should_log(.)";
  return _logger->_should_log(log_type);
}

bool Logger::_should_log(LogType log_type) noexcept {
  /* This will not work absolutely correctly in terms of parallel algorithms, but it's ok */
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

}  // namespace core