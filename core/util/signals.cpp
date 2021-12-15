#include "core/util/signals.h"

namespace core::signals {

void set_terminate_handler() {
  std::set_terminate([]() {
    LOG(ERROR) << "Panic: terminate handler called";
    std::exception_ptr exception_ptr = std::current_exception();
    try {
      std::rethrow_exception(exception_ptr);
    } catch (std::exception& exception) {
      std::fprintf(stderr, "\tUnhandled exception: %s\n", exception.what());
    } catch (...) {
      std::fprintf(stderr, "\tUnhandled exception of unknown type\n");
    }
    LOG(FATAL) << "Terminated";
  });
}

}  // namespace core::signals