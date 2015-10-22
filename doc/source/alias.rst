=====
alias
=====

Include
=======

.. code-block:: cpp

    #include <archie/alias.hpp>

Examples
========

Rebinding references
--------------------

.. code-block:: cpp
    :linenos:

    int x = 0;
    int y = 1;
    auto a = alias(x);
    REQUIRE(unwrap(a) == 0);
    a = 2;
    REQUIRE(x == 2);
    a = rebind(y);
    a = 3;
    REQUIRE(x == 2);
    REQUIRE(y == 3);

Cross-const rebinding
---------------------

.. code-block:: cpp
    :linenos:

    const int ci = 1;
    int i = 2;
    auto ca = alias(ci);
    REQUIRE(unwrap(ca) == 1);
    ca = rebind(i);
    REQUIRE(unwrap(ca) == 2);

Sorting by address
------------------

.. code-block:: cpp
    :linenos:

    std::array<int, 3> array = {{3, 1, 2}};
    std::vector<alias_t<int>> vec;
    vec.emplace_back(array[1]);
    vec.emplace_back(array[2]);
    vec.emplace_back(array[0]);

    std::sort(std::begin(vec), std::end(vec));
    REQUIRE(unwrap(vec[0]) == 3);
    REQUIRE(unwrap(vec[1]) == 1);
    REQUIRE(unwrap(vec[2]) == 2);

    std::sort(std::begin(vec), std::end(vec),
      [](auto const& lhs, auto const& rhs) {
        return unwrap(lhs) < unwrap(rhs);
      }
    );
    REQUIRE(unwrap(vec[0]) == 1);
    REQUIRE(unwrap(vec[1]) == 2);
    REQUIRE(unwrap(vec[2]) == 3);

API Reference
=============

rebind_t<T>
-----------

.. cpp:class:: rebind_t<T>

    .. cpp:type:: reference
    .. cpp:function:: explicit rebind_t(reference x) noexcept
    .. cpp:function:: explicit rebind_t(rebind_t<U>) noexcept

alias_t<T>
----------

.. cpp:class:: alias_t<T>

    .. cpp:type:: value_type
    .. cpp:type:: pointer
    .. cpp:type:: const_pointer
    .. cpp:type:: reference
    .. cpp:type:: const_reference

    .. cpp:function:: alias_t() = delete
    .. cpp:function:: alias_t(value_type&&) = delete
    .. cpp:function:: alias_t& operator=(alias_t const&) = delete

    .. cpp:function:: alias_t(alias_t const&) noexcept = default
    .. cpp:function:: alias_t(alias_t&&) noexcept = default
    .. cpp:function:: alias_t& operator=(alias_t&&) noexcept = default

    .. cpp:function:: explicit alias_t(rebind_t<U>)
    .. cpp:function:: explicit alias_t(reference) noexcept
    .. cpp:function:: alias_t(alias_t<U> const&) noexcept

    .. cpp:function:: alias_t<T>& operator=(rebind_t<U>) noexcept
    .. cpp:function:: alias_t<T>& operator=(const_reference)
    .. cpp:function:: alias_t<T>& operator=(value_type&&)

    .. cpp:function:: reference operator*() noexcept
    .. cpp:function:: const_reference operator*() const noexcept

    .. cpp:function:: pointer operator->() noexcept
    .. cpp:function:: const_pointer operator->() const noexcept

Comparison operators
--------------------

Compare address of captured references.

.. cpp:function:: bool operator==(alias_t<T> const&, alias_t<T> const&)
.. cpp:function:: bool operator!=(alias_t<T> const&, alias_t<T> const&)
.. cpp:function:: bool operator<(alias_t<T> const&, alias_t<T> const&)
.. cpp:function:: bool operator>(alias_t<T> const&, alias_t<T> const&)
.. cpp:function:: bool operator<=(alias_t<T> const&, alias_t<T> const&)
.. cpp:function:: bool operator>=(alias_t<T> const&, alias_t<T> const&)

Free function objects
---------------------

.. cpp:function:: alias_t<T> alias(T&) noexcept
.. cpp:function:: rebind_t<T> rebind(T&) noexcept
.. cpp:function:: rebind_t<T> rebind(alias_t<T>) noexcept
.. cpp:function:: T& unwrap(rebind_t<T>) noexcept
.. cpp:function:: T& unwrap(alias_t<T>) noexcept

