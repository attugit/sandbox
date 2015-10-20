=====
alias
=====

Include
=======

.. code-block:: cpp

    #include <archie/alias.hpp>

Examples
========

.. code-block:: cpp
    :linenos:

    std::array<int, 3> array = {{3, 1, 2}};
    std::vector<alias_t<int>> vec;
    vec.emplace_back(array[1]);
    vec.emplace_back(array[2]);
    vec.emplace_back(array[0]);

    std::sort(std::begin(vec), std::end(vec));
    REQUIRE(*(vec[0]) == 3);
    REQUIRE(*(vec[1]) == 1);
    REQUIRE(*(vec[2]) == 2);

    std::sort(std::begin(vec), std::end(vec),
      [](auto const& lhs, auto const& rhs) {
        return *lhs < *rhs;
      }
    );

    REQUIRE(*(vec[0]) == 1);
    REQUIRE(*(vec[1]) == 2);
    REQUIRE(*(vec[2]) == 3);

API Reference
=============

alias_t<T>
----------

.. cpp:class:: alias_t<T>

    .. cpp:type:: value_type
    .. cpp:type:: pointer
    .. cpp:type:: const_pointer
    .. cpp:type:: reference
    .. cpp:type:: const_reference
    .. cpp:class:: rebind_t

        .. cpp:function:: rebind_t(reference)
        .. cpp:function:: rebind_t(rebind_t const&) = default

    .. cpp:function:: alias_t<T>() = delete
    .. cpp:function:: alias_t<T>(value_type &&) = delete
    .. cpp:function:: alias_t<T>(alias_t<T> const&) = default
    .. cpp:function:: alias_t<T>(alias_t<T> &&) = default
    .. cpp:function:: alias_t<T>(alias_t<U> const&)
    .. cpp:function:: explicit alias_t<T>(rebind_t)

        :param rebind_t: rebind to create alias_t from

    .. cpp:function:: explicit alias_t<T>(reference)

        :param reference: reference to capture in alias_t

    .. cpp:function:: alias_t<T>& operator=(alias_t<T> const&) = delete
    .. cpp:function:: alias_t<T>& operator=(alias_t<T>&&) = default
    .. cpp:function:: alias_t<T>& operator=(rebind_t)
    .. cpp:function:: alias_t<T>& operator=(const_reference)
    .. cpp:function:: alias_t<T>& operator=(value_type&&)
    .. cpp:function:: reference get()
    .. cpp:function:: const_reference get() const
    .. cpp:function:: reference operator*()
    .. cpp:function:: const_reference operator*() const
    .. cpp:function:: pointer operator->()
    .. cpp:function:: const_pointer operator->() const

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

.. cpp:type:: rebind_t<T>

.. cpp:function:: alias_t<T> alias(T&) noexcept
.. cpp:function:: rebind_t<T> rebind(T&) noexcept
.. cpp:function:: rebind_t<T> rebind(alias_t<T>) noexcept
.. cpp:function:: typename alias_t<T>::reference unwrap(alias_t<T>) noexcept

