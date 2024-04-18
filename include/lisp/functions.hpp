#pragma once

#include <lisp/utils/container_utils.hpp>
#include <lisp/utils/iterator_range.hpp>
#include <lisp/value.hpp>

namespace lisp
{

template <class Op>
struct binary
{
    Op op;

    value operator()(const std::vector<value>& args) const
    {
        return op(args.at(0), args.at(1));
    }
};

template <class Op>
binary(Op) -> binary<Op>;

struct print
{
    value operator()(const std::vector<value>& args) const
    {
        std::cout << delimit(args, " ") << "\n";
        return {};
    }
};

struct car
{
    value operator()(const std::vector<value>& args) const
    {
        return args.at(0).as_array().at(0);
    }
};

struct cdr
{
    value operator()(const std::vector<value>& args) const
    {
        const auto& a = args.at(0).as_array();
        return value::array_type{ std::next(std::begin(a)), std::end(a) };
    }
};

struct cons
{
    value operator()(const std::vector<value>& args) const
    {
        return concat(vec(args.at(0)), args.at(1).is_array() ? args.at(1).as_array() : vec(args.at(1)));
    }
};

struct list
{
    value operator()(const std::vector<value>& args) const
    {
        return args;
    }
};

struct partial
{
    value operator()(const std::vector<value>& args) const
    {
        auto fn = args.at(0).as_callable();
        std::vector<value> bound_args = iterator_range{ args } |= drop(1);
        auto func = [=](const std::vector<value>& call_args)
        {
            const std::vector<value> all_args = concat(bound_args, call_args);
            return fn(all_args);
        };
        return value::callable_type{ func, str("partial func=", args[0], ", bound_args=[", delimit(bound_args, ", "), "]") };
    }
};

struct pipe
{
    value operator()(const std::vector<value>& args) const
    {
        auto func = [=](const std::vector<value>& call_args)
        {
            value result = args.at(0).as_callable()(call_args);
            for (const auto& fn : iterator_range{ args } |= drop(1))
            {
                result = fn.as_callable()({ result });
            }
            return result;
        };
        return value::callable_type{ func, "pipe" };
    }
};

struct callable_wrapper
{
    value::callable_type callable;

    value operator()(value arg) const
    {
        return callable({ std::move(arg) });
    }
};

struct seq_map
{
    value operator()(const std::vector<value>& args) const
    {
        const auto func = args.at(0).as_callable();
        const auto& a = args.at(1).as_array();
        array result;
        result.reserve(a.size());
        std::transform(std::begin(a), std::end(a), std::back_inserter(result), callable_wrapper{ func });
        return result;
    }
};

struct seq_filter
{
    value operator()(const std::vector<value>& args) const
    {
        const auto func = args.at(0).as_callable();
        const auto& a = args.at(1).as_array();
        array result;
        result.reserve(a.size());
        std::copy_if(std::begin(a), std::end(a), std::back_inserter(result), callable_wrapper{ func });
        return result;
    }
};

struct seq_rev
{
    value operator()(const std::vector<value>& args) const
    {
        array a = args.at(0).as_array();
        std::reverse(std::begin(a), std::end(a));
        return a;
    }
};

struct seq_at
{
    value operator()(const std::vector<value>& args) const
    {
        const auto n = args.at(0).as_integer();
        const auto& a = args.at(1).as_array();
        if (n < static_cast<value::integer_type>(a.size()))
        {
            // return array{ symbol{ "ok" }, a[n] };
            return a[n];
        }
        else
        {
            return null;
        }
    }
};

struct str_cat
{
    value operator()(const std::vector<value>& args) const
    {
        std::stringstream ss;
        for (const value& a : args)
        {
            ss << a.as_string();
        }
        return ss.str();
    }
};

struct str_has_prefix
{
    value operator()(const std::vector<value>& args) const
    {
        const auto prefix = std::string_view{ args.at(0).as_string() };
        const auto text = std::string_view{ args.at(1).as_string() };
        return text.size() >= prefix.size() && text.substr(0, prefix.size()) == prefix;
    }
};

struct str_has_suffix
{
    value operator()(const std::vector<value>& args) const
    {
        const auto suffix = std::string_view{ args.at(0).as_string() };
        const auto text = std::string_view{ args.at(1).as_string() };
        return text.size() >= suffix.size() && text.substr(text.size() - suffix.size()) == suffix;
    }
};

}  // namespace lisp
