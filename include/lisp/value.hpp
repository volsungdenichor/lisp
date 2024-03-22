#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <lisp/category.hpp>
#include <lisp/null.hpp>
#include <lisp/stack.hpp>
#include <lisp/symbol.hpp>
#include <lisp/utils/box.hpp>
#include <lisp/utils/overload.hpp>
#include <variant>

namespace lisp
{

template <class Symbol, class Value>
struct lambda_base
{
    using stack_type = stack_base<Symbol, Value>;
    Value params;
    Value body;
    stack_type stack;
};

template <class T, class... Types>
constexpr inline bool is_any_of = std::disjunction_v<std::is_same<T, Types>...>;

struct value
{
    struct callable_base
    {
        using function_type = std::function<value(const std::vector<value>&)>;
        function_type fn;
        std::string name;

        explicit callable_base(function_type fn, std::string name) : fn{ std::move(fn) }, name{ std::move(name) }
        {
        }

        value operator()(const std::vector<value>& args) const
        {
            return fn(args);
        }
    };

    using category_type = category;
    using null_type = null_t;
    using symbol_type = symbol;
    using string_type = std::string;
    using integer_type = std::int32_t;
    using boolean_type = bool;
    using floating_point_type = double;
    using callable_type = callable_base;
    using array_type = std::vector<value>;
    using lambda_type = lambda_base<symbol_type, value>;

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

    template <class T>
    static constexpr inline bool is_valid_type = is_any_of<
        T,
        null_type,
        string_type,
        symbol_type,
        integer_type,
        floating_point_type,
        boolean_type,
        array_type,
        callable_type,
        lambda_type>;

    variant_type m_data;

    value();

    template <class T, std::enable_if_t<std::is_constructible_v<variant_type, T>, int> = 0>
    value(T&& v) : m_data{ std::forward<T>(v) }
    {
    }

    value(const value&) = default;
    value(value&&) = default;

    value& operator=(value other);

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

    category type() const;

    friend std::ostream& operator<<(std::ostream& os, const value& item);

private:
    template <class... Matchers>
    decltype(auto) match(Matchers&&... matchers) const
    {
        return std::visit(overload{ std::forward<Matchers>(matchers)... }, m_data);
    }
};

value operator+(const value& lhs, const value& rhs);
value operator-(const value& lhs, const value& rhs);
value operator*(const value& lhs, const value& rhs);
value operator/(const value& lhs, const value& rhs);

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
