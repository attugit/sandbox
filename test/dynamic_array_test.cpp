#include <algorithm>

namespace archie {
template <typename>
struct array_traits;

template <typename S>
struct base_buffer {
  using storage_type = S;

private:
  using traits = array_traits<storage_type>;
  storage_type& self() { return static_cast<storage_type&>(*this); }
  storage_type const& self() const { return static_cast<storage_type const&>(*this); }

public:
  base_buffer() : end_(begin()) {}

  using reference = typename traits::reference;
  using const_reference = typename traits::const_reference;
  using iterator = typename traits::iterator;
  using const_iterator = typename traits::const_iterator;
  using size_type = typename traits::size_type;

  iterator begin() { return iterator{self().data()}; }
  const_iterator cbegin() const { return const_iterator{self().data()}; }
  const_iterator begin() const { return cbegin(); }

  iterator end() { return end_; }
  const_iterator cend() const { return end_; }
  const_iterator end() const { return cend(); }

  size_type max_size() const { return self().capacity(); }
  size_type size() const { return std::distance(cbegin(), cend()); }
  bool empty() const { return size() == 0; }

  reference operator[](size_type pos) { return *(begin() + pos); }
  const_reference operator[](size_type pos) const { return *(begin() + pos); }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    traits::construct(end_++, std::forward<Args>(args)...);
  }

  void pop_back() { traits::destroy(--end_); }

  void clear() {
    while (!empty()) pop_back();
  }

protected:
  template <typename F, typename Iterator>
  void assign(F f, iterator d_first, Iterator i_first, Iterator i_last) {
    if (d_first != i_first) {
      while (d_first != this->end() && i_first != i_last) { *d_first++ = f(*i_first++); }
      while (i_first != i_last) {
        this->emplace_back(f(*i_first++));
        ++d_first;
      }
      while (d_first != this->end()) { this->pop_back(); }
    }
  }

  iterator end_;
};

template <typename S>
bool operator==(base_buffer<S> const& lhs, base_buffer<S> const& rhs) {
  return std::equal(std::begin(lhs), std::end(lhs), std::begin(rhs), std::end(rhs));
}
template <typename S>
bool operator!=(base_buffer<S> const& lhs, base_buffer<S> const& rhs) {
  return !(lhs == rhs);
}
template <typename S>
bool operator<(base_buffer<S> const& lhs, base_buffer<S> const& rhs) {
  return std::lexicographical_compare(
      std::begin(lhs), std::end(lhs), std::begin(rhs), std::end(rhs));
}
}

#include <initializer_list>

namespace archie {
template <typename T, std::size_t N>
struct stack_buffer : base_buffer<stack_buffer<T, N>> {
private:
  using traits = array_traits<stack_buffer<T, N>>;

public:
  using value_type = typename traits::value_type;
  using pointer = typename traits::pointer;
  using const_pointer = typename traits::const_pointer;
  using reference = typename traits::reference;
  using const_reference = typename traits::const_reference;
  using size_type = typename traits::size_type;
  using iterator = typename traits::iterator;
  using const_iterator = typename traits::const_iterator;

private:
  union storage {
    storage() {}
    ~storage() {}
    char dummy;
    value_type data[N];
  } store;

public:
  stack_buffer() = default;
  stack_buffer(std::initializer_list<value_type> init) {
    for (auto const& x : init) this->emplace_back(x);
  }
  stack_buffer(stack_buffer const& other) {
    for (auto const& x : other) this->emplace_back(x);
  }
  stack_buffer(stack_buffer&& other) {
    for (auto& x : other) this->emplace_back(std::move(x));
  }
  stack_buffer& operator=(stack_buffer const& other) {
    this->assign([](const_reference r) -> const_reference { return r; },
                 this->begin(),
                 other.begin(),
                 other.end());
    return *this;
  }
  stack_buffer& operator=(stack_buffer&& other) {
    this->assign([](reference r) -> value_type && { return std::move(r); },
                 this->begin(),
                 other.begin(),
                 other.end());
    return *this;
  }
  ~stack_buffer() { this->clear(); }

  pointer data() { return &store.data[0]; }
  const_pointer data() const { return &store.data[0]; }
  size_type capacity() const { return N; }
};

template <typename T, std::size_t N>
struct array_traits<stack_buffer<T, N>> {
  using value_type = T;
  using pointer = value_type*;
  using const_pointer = value_type const*;
  using reference = value_type&;
  using const_reference = value_type const&;
  using iterator = pointer;
  using const_iterator = const_pointer;
  using size_type = std::size_t;

  template <typename... Args>
  static void construct(pointer p, Args&&... args) {
    new (p) value_type{std::forward<Args>(args)...};
  }
  static void destroy(pointer p) noexcept { p->~value_type(); }
};
}

#include <catch.hpp>
namespace {
struct resource {
  static int get_id() {
    static int id = 0;
    return id++;
  };
  explicit resource(int i) : ptr(new int(i)), id_(get_id()) {}
  resource() : resource(0) {}
  resource(resource const& r) : resource(*(r.ptr)) {}
  resource(resource&& r) : ptr(r.ptr), id_(get_id()) { r.ptr = nullptr; }
  resource& operator=(resource const& r) {
    *ptr = *r.ptr;
    return *this;
  }
  resource& operator=(resource&& r) {
    using std::swap;
    swap(ptr, r.ptr);
    return *this;
  }
  ~resource() {
    if (ptr) delete ptr;
  }
  explicit operator bool() const { return ptr != nullptr; }
  operator int() const { return *ptr; }
  int id() const { return id_; }
  int value() const { return *ptr; }
  bool operator==(resource const& r) const { return *ptr == *r.ptr; }
  bool operator!=(resource const& r) const { return !(*this == r); }

private:
  int* ptr = nullptr;
  int id_ = 0;
};

#if 0
}

#include <iostream>

namespace archie {
template <std::size_t N>
struct array_traits<stack_buffer<resource, N>> {
  using type = stack_buffer<resource, N>;
  using value_type = resource;
  using pointer = value_type*;
  using const_pointer = value_type const*;
  using reference = value_type&;
  using const_reference = value_type const&;
  using iterator = pointer;
  using const_iterator = const_pointer;
  using size_type = std::size_t;

  template <typename... Args>
  static void construct(pointer p, Args&&... args) {
    new (p) value_type{std::forward<Args>(args)...};
    std::cout << __FUNCTION__ << " resource " << p << " <" << p->id() << "|" << (*p ? p->value() : -1) << ">\n";
  }
  static void destroy(pointer p) noexcept {
    std::cout << __FUNCTION__ << " resource " << p << " <" << p->id() << "|" << (*p ? p->value() : -1) << ">\n";
    p->~value_type();
  }
};
}

namespace {
#endif
using namespace archie;
TEST_CASE("stack_buffer", "[array]") {
  using sut = stack_buffer<resource, 7>;
  using value_t = typename sut::value_type;
  SECTION("ctor") {
    sut buff;
    REQUIRE(buff.capacity() == 7);
    REQUIRE(buff.empty());
  }
  SECTION("emplace_back") {
    sut buff;
    buff.emplace_back(1);
    REQUIRE(!buff.empty());
    REQUIRE(buff.size() == 1);
    REQUIRE(buff[0] == 1);
  }
  SECTION("initializer_list") {
    sut buff = {value_t(1), value_t(2), value_t(3), value_t(4), value_t(5)};
    REQUIRE(buff.size() == 5);
    REQUIRE(buff[0] == 1);
    REQUIRE(buff[1] == 2);
    REQUIRE(buff[2] == 3);
    REQUIRE(buff[3] == 4);
    REQUIRE(buff[4] == 5);
  }
  SECTION("copy ctor") {
    sut const orig = {value_t(1), value_t(2), value_t(3)};
    sut cpy{orig};
    REQUIRE(cpy == orig);
  }
  SECTION("move ctor") {
    sut const ref = {value_t(1), value_t(2), value_t(3)};
    sut orig{ref};
    sut cpy{std::move(orig)};
    REQUIRE(cpy == ref);
  }
  SECTION("copy assignment") {
    sut const orig = {value_t(1), value_t(2), value_t(3)};
    sut cpy0;
    sut cpy1 = {value_t(4)};
    sut cpy2 = {value_t(5), value_t(6), value_t(7)};
    sut cpy3 = {value_t(8), value_t(9), value_t(10), value_t(11)};

    cpy0 = orig;
    REQUIRE(cpy0 == orig);
    cpy1 = orig;
    REQUIRE(cpy1 == orig);
    cpy2 = orig;
    REQUIRE(cpy2 == orig);
    cpy3 = orig;
    REQUIRE(cpy3 == orig);
    cpy3 = cpy3;
    REQUIRE(cpy3 == orig);
  }
  SECTION("eq cmp") {
    sut lhs = {value_t(1), value_t(2), value_t(3)};
    sut rhs;
    rhs.emplace_back(1);
    rhs.emplace_back(2);
    rhs.emplace_back(3);
    REQUIRE(lhs == rhs);
    rhs.emplace_back(4);
    REQUIRE(lhs != rhs);
  }
}
}
