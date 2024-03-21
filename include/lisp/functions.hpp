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
        return args.at(0).as<value::array_type>().at(0);
    }
};

struct cdr
{
    value operator()(const std::vector<value>& args) const
    {
        const auto& a = args.at(0).as<value::array_type>();
        return value::array_type{ std::next(std::begin(a)), std::end(a) };
    }
};

struct cons
{
    value operator()(const std::vector<value>& args) const
    {
        value::array_type res;
        res.push_back(args.at(0));
        if (const auto a = args.at(1).get_if<value::array_type>())
        {
            res.insert(std::end(res), std::begin(*a), std::end(*a));
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

value quote(value v)
{
    using namespace literals;
    return value::array_type{ "quote"_s, std::move(v) };
}

}  // namespace lisp
