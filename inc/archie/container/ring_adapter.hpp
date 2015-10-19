#pragma once
#include <utility>
#include <archie/container/ring_iterator.hpp>

namespace archie {
template <typename Container>
struct ring_adapter {
private:
  using container_t = Container;

public:
  using value_type = typename container_t::value_type;
  using size_type = typename container_t::size_type;
  using difference_type = typename container_t::difference_type;
  using iterator = ring_iterator<typename container_t::iterator>;
  using const_iterator = ring_iterator<typename container_t::const_iterator>;

  template <typename... Args>
  explicit ring_adapter(Args&&... args)
      : container_(std::forward<Args>(args)...), pos_(container_, 0) {}

  iterator begin() { return pos_; }
  const_iterator begin() const { return iterator{container_, 0}; }
  iterator end() { return begin() + size(); }
  const_iterator end() const { return begin() + size(); }

  size_type size() const { return container_.size(); }
  size_type capacity() const { return container_.capacity(); }
  bool empty() const { return container_.empty(); }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    if (size() != capacity()) {
      container_.emplace_back(std::forward<Args>(args)...);
      pos_ = iterator{container_, 0};
    } else { *pos_++ = std::move(value_type{std::forward<Args>(args)...}); }
  }

private:
  container_t container_;
  iterator pos_;
};
}
