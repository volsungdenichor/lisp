#pragma once

#include <iterator>
#include <lisp/utils/pipeline.hpp>
#include <lisp/utils/type_traits.hpp>

namespace detail
{

template <class Category, class T>
using iterator_of_category
    = std::enable_if_t<std::is_base_of_v<Category, typename std::iterator_traits<T>::iterator_category>>;

template <class T>
using is_input_iterator_impl = iterator_of_category<std::input_iterator_tag, T>;

template <class T>
using is_forward_iterator_impl = iterator_of_category<std::forward_iterator_tag, T>;

template <class T>
using is_bidirectional_iterator_impl = iterator_of_category<std::bidirectional_iterator_tag, T>;

template <class T>
using is_random_access_iterator_impl = iterator_of_category<std::random_access_iterator_tag, T>;

}  // namespace detail

template <class T>
struct is_input_iterator : is_detected<detail::is_input_iterator_impl, T>
{
};

template <class T>
using iterator_t = decltype(std::begin(std::declval<T&>()));

template <class T>
struct is_range : is_detected<iterator_t, T>
{
};

template <class T>
struct is_forward_iterator : is_detected<detail::is_forward_iterator_impl, T>
{
};

template <class T>
struct is_bidirectional_iterator : is_detected<detail::is_bidirectional_iterator_impl, T>
{
};

template <class T>
struct is_random_access_iterator : is_detected<detail::is_random_access_iterator_impl, T>
{
};

template <class T>
struct is_input_range : is_input_iterator<iterator_t<T>>
{
};

template <class T>
struct is_forward_range : is_forward_iterator<iterator_t<T>>
{
};

template <class T>
struct is_bidirectional_range : is_bidirectional_iterator<iterator_t<T>>
{
};

template <class T>
struct is_random_access_range : is_random_access_iterator<iterator_t<T>>
{
};

template <class T>
using iterator_t = decltype(std::begin(std::declval<T&>()));

template <class T>
using iter_category_t = typename std::iterator_traits<T>::iterator_category;

template <class T>
using iter_reference_t = typename std::iterator_traits<T>::reference;

template <class T>
using iter_value_t = typename std::iterator_traits<T>::value_type;

template <class T>
using iter_difference_t = typename std::iterator_traits<T>::difference_type;

template <class T>
using range_category_t = iter_category_t<iterator_t<T>>;

template <class T>
using range_reference_t = iter_reference_t<iterator_t<T>>;

template <class T>
using range_value_t = iter_value_t<iterator_t<T>>;

template <class T>
using range_difference_t = iter_difference_t<iterator_t<T>>;

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

    template <class Container>
    operator Container() const
    {
        return Container{ begin(), end() };
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
iterator_range(Range&&) -> iterator_range<iterator_t<Range>>;

namespace detail
{

template <class Iter>
constexpr Iter advance(Iter it, iter_difference_t<Iter> n, Iter sentinel)
{
    if constexpr (is_random_access_iterator<Iter>::value)
    {
        return std::next(it, std::min(n, std::distance(it, sentinel)));
    }
    else
    {
        while (n > 0 && it != sentinel)
        {
            ++it;
            --n;
        }
        return it;
    }
}

template <class Iter, class Pred>
constexpr Iter advance_while(Iter it, Pred pred, Iter sentinel)
{
    while (it != sentinel && std::invoke(pred, *it))
    {
        ++it;
    }
    return it;
}

struct take_fn
{
    struct impl
    {
        std::ptrdiff_t n;

        template <class Iter>
        auto operator()(iterator_range<Iter> range) const
        {
            const auto b = std::begin(range);
            const auto e = std::end(range);
            return iterator_range<Iter>{ b, advance(b, n, e) };
        }
    };

    auto operator()(std::ptrdiff_t n) const
    {
        return fn(impl{ n });
    }
};

struct drop_fn
{
    struct impl
    {
        std::ptrdiff_t n;

        template <class Iter>
        auto operator()(iterator_range<Iter> range) const
        {
            const auto b = std::begin(range);
            const auto e = std::end(range);
            return iterator_range<Iter>{ advance(b, n, e), e };
        }
    };

    auto operator()(std::ptrdiff_t n) const
    {
        return fn(impl{ n });
    }
};

struct drop_back_fn
{
    struct impl
    {
        std::ptrdiff_t n;

        template <class Iter>
        auto operator()(iterator_range<Iter> range) const
        {
            const auto b = std::begin(range);
            const auto e = std::end(range);
            return iterator_range<Iter>{ b, std::prev(e, n) };
        }
    };

    auto operator()(std::ptrdiff_t n) const
    {
        return fn(impl{ n });
    }
};

struct take_back_fn
{
    struct impl
    {
        std::ptrdiff_t n;

        template <class Iter>
        auto operator()(iterator_range<Iter> range) const
        {
            const auto b = std::begin(range);
            const auto e = std::end(range);
            return iterator_range<Iter>{ std::prev(e, n), e };
        }
    };

    auto operator()(std::ptrdiff_t n) const
    {
        return fn(impl{ n });
    }
};
}  // namespace detail

static constexpr inline auto drop = detail::drop_fn{};
static constexpr inline auto take = detail::take_fn{};
static constexpr inline auto drop_back = detail::drop_back_fn{};
static constexpr inline auto take_back = detail::take_back_fn{};
