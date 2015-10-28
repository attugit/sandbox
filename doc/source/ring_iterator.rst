=============
ring_iterator
=============

API Reference
=============

.. cpp:class:: ring_iterator<Iterator>

    .. cpp:type:: pointer
    .. cpp:type:: reference
    .. cpp:type:: difference_type

    .. cpp:function:: explicit ring_iterator(Iter, Distance0, Distance1)
    .. cpp:function:: explicit ring_iterator(Iter, Iter, Distance)
    .. cpp:function:: explicit ring_iterator(Iter, Iter)
    .. cpp:function:: explicit ring_iterator(Iter, Iter, Iter)
    .. cpp:function:: explicit ring_iterator(Range&, Distance)
    .. cpp:function:: explicit ring_iterator(Range&)
    .. cpp:function:: ring_iterator()
    .. cpp:function:: ring_iterator(ring_iterator const&) = default
    .. cpp:function:: ring_iterator(ring_iterator&&) = default
    .. cpp:function:: ring_iterator& operator=(ring_iterator const&) = default
    .. cpp:function:: ring_iterator& operator=(ring_iterator&&) = default
    .. cpp:function:: reference operator[](difference_type) const
    .. cpp:function:: reference operator*() const
    .. cpp:function:: pointer operator->() const

.. cpp:function:: ring_iterator<Iterator>& operator+=(ring_iterator<Iterator>&, Distance)
.. cpp:function:: Distance operator-(ring_iterator<Iterator> const&, ring_iterator<Iterator> const&)
.. cpp:function:: bool operator==(ring_iterator<Iterator> const&, ring_iterator<Iterator> const&)
.. cpp:function:: bool operator<(ring_iterator<Iterator> const&, ring_iterator<Iterator> const&)
