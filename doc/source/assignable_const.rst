================
assignable_const
================

Include
=======

.. code-block:: cpp

    #include <archie/assignable_const.hpp>

Examples
========

Copy-assignable class with const member
---------------------------------------

.. code-block:: cpp
    :linenos:

    struct block_t {
      block_t(unsigned n) : number(n) {}
      assignable_const<unsigned> number;
    };

    block_t b1(3);
    block_t b2(4);

    REQUIRE(b1.number == 3);
    b1 = b2;
    REQUIRE(b1.number == 4);

    REQUIRE(std::is_copy_constructible<block_t>::value);
    REQUIRE(std::is_copy_assignable<block_t>::value);

API Reference
=============

assignable_const<T>
-------------------

.. cpp:class:: assignable_const<T>

    .. cpp:type:: const_reference
    .. cpp:function:: explicit assignable_const(Args&&...)
    .. cpp:function:: assignable_const(assignable_const const&) = default
    .. cpp:function:: assignable_const(assignable_const&&) = default
    .. cpp:function:: assignable_const& operator=(assignable_const const&) = default
    .. cpp:function:: assignable_const& operator=(assignable_const&&) = default
    .. cpp:function:: operator const_reference() const

