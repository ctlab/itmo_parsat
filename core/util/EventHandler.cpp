#include "core/util/EventHandler.h"

namespace core::event {

namespace _details {

EventCallbackHandle::EventCallbackHandle(
    std::mutex* unlink_mutex, event_callback_t&& event_callback)
    : _unlink_mutex(unlink_mutex), _callback(std::move(event_callback)) {}

EventCallbackHandle::~EventCallbackHandle() {
  detach();
}

void EventCallbackHandle::detach() {
  if (_unlink_mutex) {
    std::lock_guard<std::mutex> lg(*_unlink_mutex);
    unlink();
    _unlink_mutex = nullptr;
  }
}

}  // namespace _details

EventHandler::EventHandler() : _event_thread([this] { _event_handling_thread(); }) {}

EventHandler::~EventHandler() {
  _shutdown = true;
  _event_cv.notify_one();
  _event_thread.join();
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
  auto handle_ptr = std::make_shared<_details::EventCallbackHandle>(
      &event_entry.first, std::move(event_callback));
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
    _event_cv.wait(ul, [this] { return !_event_queue.empty() || static_cast<bool>(_shutdown); });
    if (_shutdown) {
      break;
    }
    if (_event_queue.empty()) {
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

void raise(Event event) {
  EventHandler::instance().raise(event);
}

EventCallbackHandle attach(event_callback_t callback, Event event) {
  return EventHandler::instance().attach(std::move(callback), event);
}

}  // namespace core::event