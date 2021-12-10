#ifndef ITMO_PARSAT_CACHE_H
#define ITMO_PARSAT_CACHE_H

#include <unordered_map>
#include <glog/logging.h>

namespace ea {

template <typename Key, typename Value>
class Cache {
 public:
  explicit Cache(size_t max_cache_size)
      : max_cache_size_(max_cache_size) {
    CHECK_GT(max_cache_size, 0ULL) << "Cache with size 0 is not supported.";
  }

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

 private:
  size_t max_cache_size_;
  std::unordered_map<Key, Value> map_;
};

}  // namespace ea

#endif  // ITMO_PARSAT_CACHE_H
