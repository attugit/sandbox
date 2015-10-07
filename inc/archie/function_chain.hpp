#pragma once
#include <vector>

namespace archie {
template <typename...>
struct function_chain;

template <typename R, typename... Args>
struct function_chain<R(Args...)> : std::vector<R (*)(Args...)> {
private:
  using fptr_t = R (*)(Args...);
  using base_t = std::vector<fptr_t>;

public:
  using base_t::base_t;
  template <typename F>
  void push_back(F f) {
    static_assert(std::is_empty<F>::value == true, "");
    base_t::push_back(f);
  }
};
}
