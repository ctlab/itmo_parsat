#include "evol/include/limit/SignalLimit.h"

#include <glog/logging.h>

#include <csignal>

#include "evol/include/util/Registry.h"

namespace {

int str_to_sig(std::string const& s) {
  if (s == "SIGABRT") {
    return SIGABRT;
  } else if (s == "SIGTERM") {
    return SIGTERM;
  } else if (s == "SIGINT") {
    return SIGINT;
  } else {
    LOG(FATAL) << "Signal not supported: " << s;
  }
}

volatile bool registered_ = false;

extern "C" void handler(int sig) {
  LOG(INFO) << "SignalLimit: caught signal " << strsignal(sig) << "(" << sig << "), stopping algorithm.";
  registered_ = true;
}

}  // namespace

namespace ea::limit {

void SignalLimit::set_handlers(void (*f)(int)) {
  LOG(INFO) << "SignalLimit: setting handlers.";
  for (int i = 0; i < cfg_.sig_type_size(); ++i) {
    std::signal(str_to_sig(cfg_.sig_type(i)), f);
  }
}

SignalLimit::SignalLimit(SignalLimitConfig config)
  : cfg_(std::move(config)) {
  set_handlers(handler);
}

bool SignalLimit::proceed(instance::RPopulation) {
  if (registered_) {
    set_handlers(SIG_DFL);
  }
  return !registered_;
}

REGISTER_PROTO(Limit, SignalLimit, signal_limit_config);

}  // namespace ea::limit