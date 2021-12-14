#ifndef ITMO_PARSAT_CACHE_H
#define ITMO_PARSAT_CACHE_H

#include <unordered_map>
#include <glog/logging.h>

namespace core {

template <typename Key, typename Value>
class Cache {
 public:
  explicit Cache(size_t max_cache_size = 0) : max_cache_size_(max_cache_size) {}

  void add(Key const& key, Value const& value) {
    if (map_.size() == max_cache_size_) {
      map_.erase(map_.begin());
    }
    map_[key] = value;
  }

  std::optional<Value> get(Key const& key) {
    auto it = map_.find(key);
    return it == map_.end() ? std::optional<Value>() : it->second;
  }

  void invalidate() {
    map_.clear();
  }

  void set_max_size(size_t new_max_size) {
    max_cache_size_ = new_max_size;
  }

 private:
  size_t max_cache_size_;
  std::unordered_map<Key, Value> map_;
};

}  // namespace core

#endif  // ITMO_PARSAT_CACHE_H
