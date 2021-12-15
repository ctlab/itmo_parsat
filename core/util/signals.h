#ifndef ITMO_PARSAT_SIGNALS_H
#define ITMO_PARSAT_SIGNALS_H

#include <iostream>
#include <functional>
#include <thread>
#include <atomic>
#include <csignal>
#include <glog/logging.h>

namespace core::signals {

void set_terminate_handler();

}  // namespace core::signals


#endif  // ITMO_PARSAT_SIGNALS_H
