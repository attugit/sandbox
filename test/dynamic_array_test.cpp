#include <archie/container/stack_buffer.hpp>

#include <memory>
namespace archie {
namespace detail {
  template <typename Buffer, std::size_t N, typename Alloc>
  struct storage_t : Alloc {
    using size_type = typename Buffer::size_type;
    using pointer = typename Buffer::pointer;
    using const_pointer = typename Buffer::const_pointer;
    using value_type = typename Buffer::value_type;

  private:
    pointer data_ = nullptr;
    union u {
      u() {}
      ~u() {}
      size_type capacity_;
      value_type stack_[N];
    } u_;

  public:
    storage_t() : data_(&(u_.stack_[0])) {}
    explicit storage_t(size_type S) : data_(S > N ? Alloc::allocate(S) : &(u_.stack_[0])) {
      if (is_on_heap()) u_.capacity_ = S;
    }
    storage_t(storage_t&& orig) : storage_t() { *this = std::move(orig); }
    storage_t& operator=(storage_t&& orig) {
      if (orig.is_on_heap()) {
        using std::swap;
        swap(data_, orig.data_);
        swap(u_.capacity_, orig.u_.capacity_);
        orig.data_ = &(orig.u_.stack_[0]);
      }
      return *this;
    }
    ~storage_t() {
      if (is_on_heap()) Alloc::deallocate(data_, capacity());
    }

    bool is_on_heap() const { return data_ != &(u_.stack_[0]); }
    pointer data() { return data_; }
    const_pointer data() const { return data_; }
    size_type capacity() const { return is_on_heap() ? u_.capacity_ : N; }
    void realloc(size_type S) {
      if (is_on_heap()) Alloc::deallocate(data_, capacity());
      data_ = S > N ? Alloc::allocate(S) : &(u_.stack_[0]);
      if (is_on_heap()) u_.capacity_ = S;
    }
  };

  template <typename Buffer, typename Alloc>
  struct storage_t<Buffer, 0, Alloc> : Alloc {
    using size_type = typename Buffer::size_type;
    using pointer = typename Buffer::pointer;
    using const_pointer = typename Buffer::const_pointer;

    storage_t() : storage_t(0) {}
    explicit storage_t(size_type S)
        : data_(S > 0 ? Alloc::allocate(S) : nullptr), capacity_(data_ != nullptr ? S : 0) {}
    storage_t(storage_t&& orig) : storage_t(0) { *this = std::move(orig); }
    storage_t& operator=(storage_t&& orig) {
      using std::swap;
      swap(data_, orig.data_);
      swap(capacity_, orig.capacity_);
      return *this;
    }
    ~storage_t() {
      if (data() != nullptr && capacity() > 0) Alloc::deallocate(data(), capacity());
    }

    constexpr bool is_on_heap() const { return true; }
    pointer data() { return data_; }
    const_pointer data() const { return data_; }
    size_type capacity() const { return capacity_; }
    void realloc(size_type S) {
      if (data() != nullptr && capacity() > 0) Alloc::deallocate(data(), capacity());
      data_ = S > 0 ? Alloc::allocate(S) : nullptr;
      capacity_ = data_ != nullptr ? S : 0;
    }

  private:
    pointer data_ = nullptr;
    size_type capacity_ = 0;
  };
}

template <typename T, typename Alloc = std::allocator<T>>
struct heap_buffer : base_buffer<heap_buffer<T, Alloc>> {
private:
  using base_t = base_buffer<heap_buffer<T, Alloc>>;
  using traits = array_traits<heap_buffer<T, Alloc>>;

public:
  using value_type = typename traits::value_type;
  using pointer = typename traits::pointer;
  using const_pointer = typename traits::const_pointer;
  using reference = typename traits::reference;
  using const_reference = typename traits::const_reference;
  using size_type = typename traits::size_type;

private:
  using storage_t = detail::storage_t<heap_buffer<T, Alloc>, 0, Alloc>;
  storage_t storage_;

  void realloc(size_type S) {
    this->clear();
    storage_.realloc(S);
    this->reset();
  }

public:
  heap_buffer() : base_t(), storage_() { this->reset(); }
  explicit heap_buffer(size_type S) : base_t(nullptr), storage_(S) { this->reset(); }
  heap_buffer(std::initializer_list<value_type> init) : heap_buffer(init.size()) {
    for (auto const& x : init) this->emplace_back(x);
  }
  heap_buffer(heap_buffer const& orig) : heap_buffer(orig.size()) {
    for (auto const& x : orig) this->emplace_back(x);
  }
  heap_buffer(heap_buffer&& orig) : base_t(orig.end()), storage_() {
    using std::swap;
    swap(this->storage_, orig.storage_);
    orig.reset();
  }
  heap_buffer& operator=(heap_buffer const& orig) {
    if (this->capacity() != orig.capacity()) this->realloc(orig.capacity());
    this->assign([](const_reference r) -> const_reference { return r; },
                 this->begin(),
                 orig.begin(),
                 orig.end());
    return *this;
  }
  heap_buffer& operator=(heap_buffer&& orig) {
    using std::swap;
    swap(this->storage_, orig.storage_);
    this->end_ = orig.end_;
    orig.reset();
    return *this;
  }
  ~heap_buffer() { this->clear(); }

  pointer data() { return this->storage_.data(); }
  const_pointer data() const { return this->storage_.data(); }
  size_type capacity() const { return this->storage_.capacity(); }
};

template <typename T, typename Alloc>
struct array_traits<heap_buffer<T, Alloc>>
    : base_factory<typename Alloc::value_type, typename Alloc::pointer> {
  using value_type = typename Alloc::value_type;
  using pointer = typename Alloc::pointer;
  using const_pointer = typename Alloc::const_pointer;
  using reference = typename Alloc::reference;
  using const_reference = typename Alloc::const_reference;
  using size_type = typename Alloc::size_type;
  using iterator = pointer;
  using const_iterator = const_pointer;
};
}

namespace archie {
template <typename T, std::size_t N, typename Alloc = std::allocator<T>>
struct mixed_buffer : base_buffer<mixed_buffer<T, N, Alloc>> {
private:
  using base_t = base_buffer<mixed_buffer<T, N, Alloc>>;
  using traits = array_traits<mixed_buffer<T, N, Alloc>>;

public:
  using value_type = typename traits::value_type;
  using pointer = typename traits::pointer;
  using const_pointer = typename traits::const_pointer;
  using reference = typename traits::reference;
  using const_reference = typename traits::const_reference;
  using size_type = typename traits::size_type;

private:
  using storage_t = detail::storage_t<mixed_buffer<T, N, Alloc>, N, Alloc>;
  storage_t storage_;

  void realloc(size_type S) {
    this->clear();
    storage_.realloc(S);
    this->reset();
  }

public:
  mixed_buffer() : base_t() { this->reset(); }
  explicit mixed_buffer(size_type S) : base_t(nullptr), storage_(S) { this->reset(); }
  mixed_buffer(std::initializer_list<value_type> init) : mixed_buffer(init.size()) {
    for (auto const& x : init) this->emplace_back(x);
  }
  mixed_buffer(mixed_buffer const& orig) : mixed_buffer(orig.size()) {
    for (auto const& x : orig) this->emplace_back(x);
  }
  mixed_buffer(mixed_buffer&& orig) : mixed_buffer() { *this = std::move(orig); }
  mixed_buffer& operator=(mixed_buffer const& orig) {
    if (this->capacity() != orig.capacity()) this->realloc(orig.capacity());
    this->assign([](const_reference r) -> const_reference { return r; },
                 this->begin(),
                 orig.begin(),
                 orig.end());
    return *this;
  }
  mixed_buffer& operator=(mixed_buffer&& orig) {
    this->realloc(0);
    if (orig.storage_.is_on_heap()) {
      using std::swap;
      swap(this->storage_, orig.storage_);
      swap(this->end_, orig.end_);
      orig.reset();
    } else {
      for (auto& x : orig) this->emplace_back(std::move(x));
    }
    return *this;
  }
  ~mixed_buffer() { this->clear(); }

  pointer data() { return this->storage_.data(); }
  const_pointer data() const { return this->storage_.data(); }
  size_type capacity() const { return this->storage_.capacity(); }
};

template <typename T, std::size_t N, typename Alloc>
struct array_traits<mixed_buffer<T, N, Alloc>>
    : base_factory<typename Alloc::value_type, typename Alloc::pointer> {
  using value_type = typename Alloc::value_type;
  using pointer = typename Alloc::pointer;
  using const_pointer = typename Alloc::const_pointer;
  using reference = typename Alloc::reference;
  using const_reference = typename Alloc::const_reference;
  using size_type = typename Alloc::size_type;
  using iterator = pointer;
  using const_iterator = const_pointer;
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

using namespace archie;
TEST_CASE("stack_buffer", "[array]") {
  enum { stack_size = 7 };
  using sut = stack_buffer<resource, stack_size>;
  using value_t = typename sut::value_type;
  SECTION("ctor") {
    sut buff;
    REQUIRE(buff.capacity() == stack_size);
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
TEST_CASE("heap_buffer", "[array]") {
  using sut = heap_buffer<resource>;
  using value_t = typename sut::value_type;
  SECTION("default ctor") {
    sut buff;
    REQUIRE(buff.capacity() == 0);
    REQUIRE(buff.empty());
  }
  SECTION("ctor") {
    sut buff(7);
    REQUIRE(buff.capacity() == 7);
    REQUIRE(buff.empty());
  }
  SECTION("emplace_back") {
    sut buff(7);
    buff.emplace_back(1);
    REQUIRE(!buff.empty());
    REQUIRE(buff.size() == 1);
    REQUIRE(buff[0] == 1);
  }
  SECTION("initializer_list") {
    sut buff = {value_t(1), value_t(2), value_t(3)};
    REQUIRE(buff.size() == 3);
    REQUIRE(buff[0] == 1);
    REQUIRE(buff[1] == 2);
    REQUIRE(buff[2] == 3);
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
    REQUIRE(orig.empty());
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
    sut rhs(5);
    rhs.emplace_back(1);
    rhs.emplace_back(2);
    rhs.emplace_back(3);
    REQUIRE(lhs == rhs);
    rhs.emplace_back(4);
    REQUIRE(lhs != rhs);
  }
}
TEST_CASE("mixed_buffer", "[array]") {
  enum { stack_size = 7 };
  using sut = mixed_buffer<resource, stack_size>;
  using value_t = typename sut::value_type;
  SECTION("default ctor") {
    sut buff;
    REQUIRE(buff.empty());
    REQUIRE(buff.capacity() == stack_size);
  }
  SECTION("ctor") {
    sut buff_s(stack_size - 1);
    REQUIRE(buff_s.empty());
    REQUIRE(buff_s.capacity() == stack_size);

    sut buff_h(stack_size + 1);
    REQUIRE(buff_h.empty());
    REQUIRE(buff_h.capacity() == stack_size + 1);
  }
  SECTION("initializer_list") {
    sut buff_s = {value_t(1), value_t(2), value_t(3), value_t(4), value_t(5)};
    REQUIRE(buff_s.size() == 5);
    REQUIRE(buff_s.capacity() == stack_size);
    REQUIRE(buff_s[0] == 1);
    REQUIRE(buff_s[1] == 2);
    REQUIRE(buff_s[2] == 3);
    REQUIRE(buff_s[3] == 4);
    REQUIRE(buff_s[4] == 5);

    sut buff_h = {value_t(1),
                  value_t(2),
                  value_t(3),
                  value_t(4),
                  value_t(5),
                  value_t(6),
                  value_t(7),
                  value_t(8)};
    REQUIRE(buff_h.size() == 8);
    REQUIRE(buff_h.capacity() == 8);
    REQUIRE(buff_h[0] == 1);
    REQUIRE(buff_h[1] == 2);
    REQUIRE(buff_h[2] == 3);
    REQUIRE(buff_h[3] == 4);
    REQUIRE(buff_h[4] == 5);
    REQUIRE(buff_h[5] == 6);
    REQUIRE(buff_h[6] == 7);
    REQUIRE(buff_h[7] == 8);
  }
  SECTION("copy ctor") {
    sut const orig_s = {value_t(1), value_t(2), value_t(3)};
    sut cpy_s{orig_s};
    REQUIRE(cpy_s == orig_s);

    sut const orig_h = {value_t(1),
                        value_t(2),
                        value_t(3),
                        value_t(4),
                        value_t(5),
                        value_t(6),
                        value_t(7),
                        value_t(8)};
    sut cpy_h{orig_h};
    REQUIRE(cpy_h == orig_h);
  }
  SECTION("move ctor") {
    sut const ref_s = {value_t(1), value_t(2), value_t(3)};
    sut orig_s{ref_s};
    sut cpy_s{std::move(orig_s)};
    REQUIRE(cpy_s == ref_s);
    REQUIRE(orig_s.capacity() == stack_size);
    REQUIRE(orig_s.size() == ref_s.size());

    sut const ref_h = {value_t(1),
                       value_t(2),
                       value_t(3),
                       value_t(4),
                       value_t(5),
                       value_t(6),
                       value_t(7),
                       value_t(8)};
    sut orig_h{ref_h};
    REQUIRE(orig_h.capacity() > stack_size);
    sut cpy_h{std::move(orig_h)};
    REQUIRE(cpy_h == ref_h);
    REQUIRE(orig_h.capacity() == stack_size);
    REQUIRE(orig_h.empty());
  }
  auto const make_sut = [](sut::size_type S) {
    sut ret(S);
    for (auto idx = 0u; idx < S; ++idx) { ret.emplace_back(static_cast<int>(idx)); }
    return ret;
  };
  SECTION("copy assignment stack buffer") {
    sut const orig_s = {value_t(100), value_t(101), value_t(102)};
    sut cpy0;
    sut cpy1 = make_sut(1);
    sut cpy2 = make_sut(3);
    sut cpy3 = make_sut(6);
    sut cpy4 = make_sut(8);
    sut cpy5 = make_sut(9);

    cpy0 = orig_s;
    REQUIRE(cpy0 == orig_s);
    cpy1 = orig_s;
    REQUIRE(cpy1 == orig_s);
    cpy2 = orig_s;
    REQUIRE(cpy2 == orig_s);
    cpy3 = orig_s;
    REQUIRE(cpy3 == orig_s);
    cpy4 = orig_s;
    REQUIRE(cpy4 == orig_s);
    cpy5 = orig_s;
    REQUIRE(cpy5 == orig_s);

    cpy5 = cpy5;
    REQUIRE(cpy5 == orig_s);
  }
  SECTION("copy assignment heap buffer") {
    sut const orig_s = {value_t(100),
                        value_t(101),
                        value_t(102),
                        value_t(103),
                        value_t(104),
                        value_t(105),
                        value_t(106),
                        value_t(107)};
    sut cpy0;
    sut cpy1 = make_sut(1);
    sut cpy2 = make_sut(3);
    sut cpy3 = make_sut(6);
    sut cpy4 = make_sut(8);
    sut cpy5 = make_sut(9);

    cpy0 = orig_s;
    REQUIRE(cpy0 == orig_s);
    cpy1 = orig_s;
    REQUIRE(cpy1 == orig_s);
    cpy2 = orig_s;
    REQUIRE(cpy2 == orig_s);
    cpy3 = orig_s;
    REQUIRE(cpy3 == orig_s);
    cpy4 = orig_s;
    REQUIRE(cpy4 == orig_s);
    cpy5 = orig_s;
    REQUIRE(cpy5 == orig_s);

    cpy5 = cpy5;
    REQUIRE(cpy5 == orig_s);
  }

  SECTION("eq cmp") {
    sut lhs = {value_t(1), value_t(2), value_t(3)};
    sut rhs(5);
    rhs.emplace_back(1);
    rhs.emplace_back(2);
    rhs.emplace_back(3);
    REQUIRE(lhs == rhs);
    rhs.emplace_back(4);
    REQUIRE(lhs != rhs);
  }
}
}
