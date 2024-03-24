#pragma once

#include <lisp/value.hpp>

namespace lisp
{

template <class Op>
struct binary
{
    Op op;

    value operator()(const std::vector<value>& args) const
    {
        if (args.size() != 2)
        {
            throw std::runtime_error{ str("Invalid argument number: expected ", 2, ", got ", args.size()) };
        }
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
        value::array_type res;
        res.push_back(args.at(0));
        if (args.at(1).is_array())
        {
            const auto& a = args.at(1).as_array();
            res.insert(std::end(res), std::begin(a), std::end(a));
        }
        else
        {
            res.push_back(args.at(1));
        }
        return res;
    }
};

struct list
{
    value operator()(const std::vector<value>& args) const
    {
        return args;
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
