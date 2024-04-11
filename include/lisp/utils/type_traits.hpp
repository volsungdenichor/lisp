#pragma once

#include <type_traits>

namespace detail
{
template <class AlwaysVoid, template <class...> class Op, class... Args>
struct detector_impl : std::false_type
{
};

template <template <class...> class Op, class... Args>
struct detector_impl<std::void_t<Op<Args...>>, Op, Args...> : std::true_type
{
};
}  // namespace detail

template <template <class...> class Op, class... Args>
struct is_detected : detail::detector_impl<std::void_t<>, Op, Args...>
{
};

template <template <class...> class Op, class... Args>
static constexpr inline bool is_detected_v = is_detected<Op, Args...>::value;

template <class... T>
constexpr bool always_false = false;
