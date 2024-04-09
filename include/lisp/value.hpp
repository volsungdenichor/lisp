#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <lisp/category.hpp>
#include <lisp/null.hpp>
#include <lisp/stack.hpp>
#include <lisp/symbol.hpp>
#include <lisp/utils/box.hpp>
#include <lisp/utils/container_utils.hpp>
#include <lisp/utils/overload.hpp>
#include <optional>
#include <variant>

namespace lisp
{

template <class Symbol, class Value>
struct lambda_base
{
    using stack_type = stack_base<Symbol, Value>;
    Value params;
    Value body;
    stack_type* stack;
};

template <class Value>
struct callable_base
{
    using function_type = std::function<Value(const std::vector<Value>&)>;
    function_type fn;
    std::string name;
    std::optional<std::size_t> arity;
    std::vector<Value> bound_args;

    explicit callable_base(function_type fn, std::string name, std::optional<int> arity = {})
        : fn{ std::move(fn) }
        , name{ std::move(name) }
        , arity{ arity }
        , bound_args{}
    {
    }

    explicit callable_base(const callable_base& self, std::vector<Value>&& bound_args)
        : fn{ self.fn }
        , name{ self.name }
        , arity{ self.arity }
        , bound_args(std::move(bound_args))
    {
    }

    Value call(const std::vector<Value>& args) const
    {
        if (arity)
        {
            std::vector<Value> all_args = concat(bound_args, args);
            if (all_args.size() > *arity)
            {
                throw std::runtime_error{ str("Expected ", *arity, " arguments, got ", all_args.size()) };
            }
            else if (all_args.size() < *arity)
            {
                return callable_base{ *this, std::move(all_args) };
            }
            else
            {
                return fn(all_args);
            }
        }
        else
        {
            return fn(args);
        }
    }

    Value operator()(const std::vector<Value>& args) const
    {
        try
        {
            return call(args);
        }
        catch (const std::exception& ex)
        {
            throw std::runtime_error{ str("On calling ", name, ": ", ex.what()) };
        }
    }
};

class value
{
public:
    using category_type = category;
    using null_type = null_t;
    using symbol_type = symbol;
    using string_type = std::string;
    using integer_type = std::int32_t;
    using boolean_type = bool;
    using floating_point_type = double;
    using callable_type = callable_base<value>;
    using array_type = std::vector<value>;
    using lambda_type = lambda_base<symbol_type, value>;

public:
    value();

    value(null_type v);
    value(string_type v);
    value(symbol_type v);
    value(integer_type v);
    value(floating_point_type v);
    value(boolean_type v);
    value(array_type v);
    value(callable_type v);
    value(lambda_type v);

    value(const value&) = default;
    value(value&&) = default;

    value& operator=(const value& other);

    explicit operator bool() const;

    bool is_null() const;
    bool is_string() const;
    bool is_symbol() const;
    bool is_integer() const;
    bool is_boolean() const;
    bool is_floating_point() const;
    bool is_array() const;
    bool is_callable() const;
    bool is_lambda() const;

    const null_type& as_null() const;
    const string_type& as_string() const;
    const symbol_type& as_symbol() const;
    const integer_type& as_integer() const;
    const boolean_type& as_boolean() const;
    const floating_point_type& as_floating_point() const;
    const array_type& as_array() const;
    const callable_type& as_callable() const;
    const lambda_type& as_lambda() const;

    category get_category() const;

    friend std::ostream& operator<<(std::ostream& os, const value& item);

private:
    using variant_type = std::variant<
        null_type,
        string_type,
        symbol_type,
        integer_type,
        floating_point_type,
        boolean_type,
        array_type,
        callable_type,
        box<lambda_type>>;

    variant_type m_data;
};

value operator+(const value& lhs, const value& rhs);
value operator-(const value& lhs, const value& rhs);
value operator*(const value& lhs, const value& rhs);
value operator/(const value& lhs, const value& rhs);
value operator%(const value& lhs, const value& rhs);

bool operator==(const value& lhs, const value& rhs);
bool operator!=(const value& lhs, const value& rhs);
bool operator<(const value& lhs, const value& rhs);
bool operator<=(const value& lhs, const value& rhs);
bool operator>(const value& lhs, const value& rhs);
bool operator>=(const value& lhs, const value& rhs);

using array = value::array_type;
using callable = value::callable_type;
using stack_type = stack_base<value::symbol_type, value>;

}  // namespace lisp
