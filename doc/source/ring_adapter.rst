============
ring_adapter
============

API Reference
=============

.. cpp:type:: Container

  Requires:

  .. code-block:: cpp
    :linenos:

    typename Container::value_type
    typename Container::size_type
    typename Container::difference_type
    typename Container::iterator
    typename Container::const_iterator

    iterator Container::begin()
    const_iterator Container::begin() const
    iterator Container::end()
    const_iterator Container::end() const
    size_type Container::size() const
    size_type Container::capacity() const
    template <typename... Args>
    void Container::emplace_back(Args&&...)

    value_type& value_type::operator=(value_type&&);

.. cpp:class:: ring_adapter<Container>

  .. type interface

  .. cpp:type:: value_type

    ``using value_type = typename Container::value_type``

  .. cpp:type:: size_type

    ``using size_type = typename Container::size_type``

  .. cpp:type:: difference_type

    ``using difference_type = typename Container::difference_type``

  .. cpp:type:: iterator

    ``using iterator = ring_iterator<typename Container::iterator>``

  .. cpp:type:: const_iterator

    ``using const_iterator = ring_iterator<typename Container::const_iterator>``

  .. cpp:function:: iterator begin() 
  .. cpp:function:: const_iterator begin() const
  .. cpp:function:: iterator end()
  .. cpp:function:: const_iterator end() const
  .. cpp:function:: size_type size() const
  .. cpp:function:: size_type capacity() const
  .. cpp:function:: bool empty() const

  .. cpp:function:: void emplace_back(Args&&...)

  .. cpp:function:: Container* operator->()
  .. cpp:function:: Container const* operator->() const
  .. cpp:function:: Container& operator*()
  .. cpp:function:: Container const& operator*() const

