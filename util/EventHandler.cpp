#include "EventHandler.h"

namespace util::event {

namespace _details {

EventCallbackHandle::EventCallbackHandle(std::mutex* unlink_mutex, event_callback_t&& event_callback)
    : _unlink_mutex(unlink_mutex), _callback(std::move(event_callback)) {}

EventCallbackHandle::~EventCallbackHandle() { detach(); }

void EventCallbackHandle::detach() {
  std::mutex* unlink_mutex = _unlink_mutex.load(std::memory_order_relaxed);
  std::mutex* dummy_nullptr = unlink_mutex;
  if (unlink_mutex && _unlink_mutex.compare_exchange_strong(dummy_nullptr, nullptr)) {
    std::lock_guard<std::mutex> lg(*unlink_mutex);
    unlink();
  }
}

}  // namespace _details

EventHandler::EventHandler() : _event_thread([this] { _event_handling_thread(); }) {}

EventHandler::~EventHandler() {
  _shutdown.store(true, std::memory_order_relaxed);
  _event_cv.notify_one();
  if (_event_thread.joinable()) {
    _event_thread.join();
  }
}

void EventHandler::raise(Event event) {
  {
    std::lock_guard<std::mutex> lg(_event_queue_mutex);
    _event_queue.push(event);
  }
  _event_cv.notify_one();
}

EventCallbackHandle EventHandler::attach(event_callback_t event_callback, Event event) {
  auto& event_entry = _cb_map[event];
  auto handle_ptr = std::make_shared<_details::EventCallbackHandle>(&event_entry.first, std::move(event_callback));
  {
    std::lock_guard<std::mutex> lg(event_entry.first);
    event_entry.second.push_back(*handle_ptr);
  }
  _event_cv.notify_one();
  return handle_ptr;
}

void EventHandler::_event_handling_thread() {
  for (;;) {
    std::unique_lock<std::mutex> ul(_event_queue_mutex);
    _event_cv.wait(ul, [this] { return !_event_queue.empty() || _shutdown.load(std::memory_order_relaxed); });
    if (IPS_UNLIKELY(_shutdown.load(std::memory_order_relaxed))) {
      break;
    }
    if (IPS_UNLIKELY(_event_queue.empty())) {
      continue;
    }

    Event event = _event_queue.front();
    _event_queue.pop();
    ul.unlock();

    auto& callback_list = _cb_map[event].second;
    for (auto it = callback_list.begin(); it != callback_list.end();) {
      try {
        (it++)->_callback();
      } catch (...) {
        // ignore
      }
    }
  }
}

EventHandler& EventHandler::instance() {
  static EventHandler handler;
  return handler;
}

void raise(Event event) { EventHandler::instance().raise(event); }

EventCallbackHandle attach(event_callback_t callback, Event event) {
  return EventHandler::instance().attach(std::move(callback), event);
}

}  // namespace util::event
