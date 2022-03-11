#ifndef ITMO_PARSAT_LFQUEUE_H
#define ITMO_PARSAT_LFQUEUE_H

#include <atomic>
#include <optional>

template <typename T>
class LFQueue {
  struct _Node;
  typedef _Node* node_ptr;

  struct _RootNode {
    std::atomic<node_ptr> head = nullptr;
    std::atomic<node_ptr> tail = nullptr;
  };

  struct _Node {
    std::atomic<node_ptr> next = nullptr;
    T value;
  };

 public:
  LFQueue() = default;

  ~LFQueue() noexcept {
    node_ptr node = _root->head;
    while (node != nullptr) {
      node_ptr next = node->next;
      delete node;
      node = next;
    }
  }

  void push(T&& value) {
    node_ptr new_node = _Node{nullptr, std::move(value)};
    node_ptr tail;
    node_ptr null = nullptr;
    for (;;) {
      tail = _root.tail.load();
      null = nullptr;
      if (tail->next.compare_exchange_strong(null, new_node)) {
        _root.tail.compare_exchange_strong(tail, new_node);
      } else {
        _root.tail.compare_exchange_strong(tail, tail->next.load());
      }
    }
  }

  std::optional<T> pop() noexcept {
    node_ptr head, tail, next_head;
    for (;;) {
      head = _root.head.load();
      tail = _root.tail.load();
      next_head = head->next.load();
      if (head == tail) {
        if (next_head == nullptr) {
          return std::nullopt;
        } else {
          _root.tail.compare_exchange_strong(tail, next_head);
        }
      } else {
        if (_root.head.compare_exchange_strong(head, next_head)) {
          std::optional<T> result = next_head->value;
          delete head;
          return result;
        }
      }
    }
  }

 private:
  _RootNode _root{};
};

#endif  // ITMO_PARSAT_LFQUEUE_H
