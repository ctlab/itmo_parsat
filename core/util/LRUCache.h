#ifndef ITMO_PARSAT_LRUCACHE_H
#define ITMO_PARSAT_LRUCACHE_H

#include <unordered_map>

namespace core {

template <typename Key, typename Value>
class LRUCache {
 public:
  /**
   * Constructs LRU Cache with the specified maximal size.
   * @param max_cache_size max size of cache
   */
  explicit LRUCache(size_t max_cache_size = 0) : max_cache_size_(max_cache_size) {}

  /**
   * Adds <key, value> pair to the cache.
   * If size reaches maximal, unspecified <key, value> pair is removed.
   */
  void add(Key const& key, Value const& value) {
    if (map_.size() == max_cache_size_) {
      map_.erase(map_.begin());
    }
    map_[key] = value;
  }

  /**
   * Retrieves value by key from a cache.
   * Returns std::nullopt if key does not exist.
   */
  std::optional<Value> get(Key const& key) {
    auto it = map_.find(key);
    return it == map_.end() ? std::optional<Value>() : it->second;
  }

  /**
   * Completely invalidates the cache.
   */
  void invalidate() {
    map_.clear();
  }

  /**
   * Changes maximal size of the cache.
   */
  void set_max_size(size_t new_max_size) {
    max_cache_size_ = new_max_size;
  }

 private:
  size_t max_cache_size_;
  std::unordered_map<Key, Value> map_;
};

}  // namespace core

#endif  // ITMO_PARSAT_LRUCACHE_H
