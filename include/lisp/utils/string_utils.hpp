#pragma once

#include <functional>
#include <sstream>

namespace detail
{

struct ostream_manipulator : public std::function<void(std::ostream&)>
{
    using base_type = std::function<void(std::ostream&)>;
    using base_type::base_type;

    friend std::ostream& operator<<(std::ostream& os, const ostream_manipulator& item)
    {
        item(os);
        return os;
    }
};

struct str_fn
{
    template <class... Args>
    std::string operator()(const Args&... args) const
    {
        std::stringstream ss;
        (ss << ... << args);
        return std::move(ss).str();
    }
};

struct delimit_fn
{
    template <class Range>
    auto operator()(Range&& range, std::string_view separator = {}) const -> ostream_manipulator
    {
        return impl(std::begin(range), std::end(range), separator);
    }

private:
    template <class Iter>
    auto impl(Iter begin, Iter end, std::string_view separator = {}) const -> ostream_manipulator
    {
        return [=](std::ostream& os)
        {
            if (begin == end)
            {
                return;
            }

            auto b = begin;
            os << *b++;
            for (; b != end; ++b)
            {
                os << separator << *b;
            }
        };
    }
};

struct make_string_view_fn
{
    std::string_view operator()(std::string_view::iterator b, std::string_view::iterator e) const
    {
        return { std::addressof(*b), std::string_view::size_type(e - b) };
    }
};

}  // namespace detail

static constexpr inline auto delimit = detail::delimit_fn{};
static constexpr inline auto str = detail::str_fn{};
static constexpr inline auto make_string_view = detail::make_string_view_fn{};
