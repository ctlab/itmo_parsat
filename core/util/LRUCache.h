#ifndef ITMO_PARSAT_LRUCACHE_H
#define ITMO_PARSAT_LRUCACHE_H

#include <unordered_map>

namespace core {

/**
 * @brief Almost LRU cache implementaion.
 * @tparam Key key type.
 * @tparam Value value type.
 */
template <typename Key, typename Value>
class LRUCache {
 public:
  /**
   * @brief Constructs LRU Cache with the specified maximal size.
   * @param max_cache_size max size of cache
   */
  explicit LRUCache(size_t max_cache_size = 0) : max_cache_size_(max_cache_size) {}

  /**
   * @brief Adds key-value pair to the cache.
   * If size reaches maximal, unspecified key-value pair is removed.
   */
  void add(Key const& key, Value const& value) {
    if (map_.size() == max_cache_size_) {
      map_.erase(map_.begin());
    }
    map_[key] = value;
  }

  /**
   * @brief Retrieves value by key from a cache.
   * Returns std::nullopt if key does not exist.
   */
  std::optional<Value> get(Key const& key) {
    auto it = map_.find(key);
    return it == map_.end() ? std::optional<Value>() : it->second;
  }

  /**
   * @brief Completely invalidates the cache.
   */
  void invalidate() {
    map_.clear();
  }

  /**
   * @brief Changes maximal size of the cache.
   * @param new_max_size new maximal size.
   */
  void set_max_size(size_t new_max_size) {
    max_cache_size_ = new_max_size;
  }

  /**
   * @return the size of cache.
   */
  [[nodiscard]] size_t size() const noexcept {
    return map_.size();
  }

 private:
  size_t max_cache_size_;
  std::unordered_map<Key, Value> map_;
};

}  // namespace core

#endif  // ITMO_PARSAT_LRUCACHE_H
