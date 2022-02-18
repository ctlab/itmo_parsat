#ifndef ITMO_PARSAT_EVENTHANDLER_H
#define ITMO_PARSAT_EVENTHANDLER_H

#include <array>
#include <atomic>
#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <memory>
#include <boost/intrusive/list.hpp>

#include "Logger.h"

namespace core::event {

enum Event {
  INTERRUPT = 0,
  SHUTDOWN = 1,
  SBS_FOUND = 2,
  /* Do not modify */ _EVENTS_COUNT
};

typedef std::function<void()> event_callback_t;

class EventHandler;

namespace _details {

class EventCallbackHandle : public boost::intrusive::list_base_hook<
                                boost::intrusive::link_mode<boost::intrusive::auto_unlink>> {
  friend class core::event::EventHandler;

 public:
  EventCallbackHandle(std::mutex* unlink_mutex, event_callback_t&& event_callback);

  ~EventCallbackHandle();

  void detach();

 private:
  std::mutex* _unlink_mutex;
  event_callback_t _callback;
};

}  // namespace _details

typedef std::shared_ptr<_details::EventCallbackHandle> EventCallbackHandle;

/**
 * @brief Event handling routine.
 */
class EventHandler {
 private:
  using callback_entry_t = std::pair<
      std::mutex, boost::intrusive::list<
                      _details::EventCallbackHandle, boost::intrusive::constant_time_size<false>>>;

 public:
  EventHandler();
  ~EventHandler();

  /**
   * @param event_callback the callback to be attached
   * @param event the event
   */
  EventCallbackHandle attach(event_callback_t event_callback, Event events);

  /**
   * @brief Raises an event. All event are processed sequentially.
   * @param event the event to be raised.
   */
  void raise(Event event);

  static EventHandler& instance();

 private:
  void _event_handling_thread();

 private:
  /// queue routine
  std::mutex _event_queue_mutex;
  std::condition_variable _event_cv;
  std::queue<Event> _event_queue;

  std::thread _event_thread;
  std::atomic_bool _shutdown{false};
  std::array<callback_entry_t, _EVENTS_COUNT> _cb_map;
};

EventCallbackHandle attach(event_callback_t callback, Event event);

void raise(Event event);

}  // namespace core::event

#endif  // ITMO_PARSAT_EVENTHANDLER_H
