#pragma once

#include <iterator>

template <class Iter>
struct iterator_range
{
    using iterator = Iter;
    using const_iterator = iterator;
    using difference_type = typename std::iterator_traits<iterator>::difference_type;
    using reference = typename std::iterator_traits<iterator>::reference;

    iterator_range(iterator b, iterator e) : b{ std::move(b) }, e{ std::move(e) }
    {
    }

    template <class Range>
    iterator_range(Range&& range) : iterator_range{ std::begin(range), std::end(range) }
    {
    }

    iterator begin() const
    {
        return b;
    }

    iterator end() const
    {
        return e;
    }

    bool empty() const
    {
        return begin() == end();
    }

    difference_type size() const
    {
        return std::distance(begin(), end());
    }

    iterator_range drop(difference_type n) const
    {
        return iterator_range{ std::next(begin(), n), end() };
    }

    iterator_range take(difference_type n) const
    {
        return iterator_range{ begin(), std::next(begin(), n) };
    }

    reference operator[](difference_type n) const
    {
        return *std::next(begin(), n);
    }

    reference at(difference_type n) const
    {
        return *std::next(begin(), n);
    }

    iterator b, e;
};

template <class Iter>
iterator_range(Iter, Iter) -> iterator_range<Iter>;

template <class Range>
iterator_range(Range&&) -> iterator_range<decltype(std::begin(std::declval<Range>()))>;
