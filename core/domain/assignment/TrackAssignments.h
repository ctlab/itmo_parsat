#ifndef ITMO_PARSAT_TRACKASSIGNMENTS_H
#define ITMO_PARSAT_TRACKASSIGNMENTS_H

#ifndef IPS_DISABLE_HEAVY_CHECKS

#include <vector>
#include <mutex>
#include <atomic>
#include <set>

#define START_ASGN_TRACK(expected_size)    \
  std::mutex _m_asgn_track;                \
  std::atomic_bool _stop_callback{false};  \
  uint32_t _expected_size = expected_size; \
  std::set<std::vector<bool>> _uniq_asgn

#define ASGN_TRACK(asgn)                                                                           \
  do {                                                                                             \
    std::vector<bool> _cur(asgn.size());                                                           \
    for (int i = 0; i < asgn.size(); ++i) {                                                        \
      _cur[i] = Mini::sign(asgn[i]);                                                              \
    }                                                                                              \
    std::lock_guard<std::mutex> _lg(_m_asgn_track);                                                \
    IPS_VERIFY(_uniq_asgn.insert(std::move(_cur)).second == true && bool("Duplicate assignment")); \
  } while (0)

#define BREAK_ASGN_TRACK _stop_callback = true

#define END_ASGN_TRACK(do_not_check_if)                                               \
  IPS_VERIFY(                                                                         \
      (_stop_callback || (do_not_check_if) || _uniq_asgn.size() == _expected_size) && \
      bool("Unexpected assignments behaviour"))

#else

#define START_ASGN_TRACK(...)

#define ASGN_TRACK(...)

#define BREAK_ASGN_TRACK

#define END_ASGN_TRACK(...)

#endif

#endif  // ITMO_PARSAT_TRACKASSIGNMENTS_H
