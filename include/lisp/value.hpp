#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <lisp/box.hpp>
#include <lisp/category.hpp>
#include <lisp/null.hpp>
#include <lisp/overload.hpp>
#include <lisp/stack.hpp>
#include <lisp/symbol.hpp>
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
    using lambda_type = box<lambda_base<symbol_type, value>>;

    using variant_type = std::variant<
        null_type,
        string_type,
        symbol_type,
        integer_type,
        floating_point_type,
        boolean_type,
        array_type,
        callable_type,
        lambda_type>;

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

    value() : m_data{ null_type{} }
    {
    }

    template <class T, std::enable_if_t<std::is_constructible_v<variant_type, T>, int> = 0>
    value(T&& v) : m_data{ std::forward<T>(v) }
    {
    }

    value(const value&) = default;
    value(value&&) = default;

    ~value() = default;

    value& operator=(value other)
    {
        m_data.~variant_type();
        new (&m_data) variant_type{ std::move(other.m_data) };
        return *this;
    }

    template <class T>
    T& emplace(T v = {})
    {
        static_assert(is_valid_type<T>, "invalid type");
        m_data.emplace<T>(std::move(v));
        return std::get<T>(m_data);
    }

    template <class T>
    const T* get_if() const
    {
        static_assert(is_valid_type<T>, "invalid type");
        return std::get_if<T>(&m_data);
    }

    template <class T>
    bool is() const
    {
        static_assert(is_valid_type<T>, "invalid type");
        return std::holds_alternative<T>(m_data);
    }

    template <class T>
    const T& as() const
    {
        static_assert(is_valid_type<T>, "invalid type");
        ensure_type<T>();
        return std::get<T>(m_data);
    }

    template <class... Matchers>
    decltype(auto) match(Matchers&&... matchers) const
    {
        return std::visit(overload{ std::forward<Matchers>(matchers)... }, m_data);
    }

    category type() const;

    friend std::ostream& operator<<(std::ostream& os, const value& item);

private:
    template <class T>
    static category to_category()
    {
        static_assert(is_valid_type<T>, "invalid type");
        if constexpr (std::is_same_v<T, null_type>)
        {
            return category::null;
        }
        if constexpr (std::is_same_v<T, string_type>)
        {
            return category::string;
        }
        if constexpr (std::is_same_v<T, symbol_type>)
        {
            return category::symbol;
        }
        if constexpr (std::is_same_v<T, integer_type>)
        {
            return category::integer;
        }
        if constexpr (std::is_same_v<T, floating_point_type>)
        {
            return category::floating_point;
        }
        if constexpr (std::is_same_v<T, boolean_type>)
        {
            return category::boolean;
        }
        if constexpr (std::is_same_v<T, array_type>)
        {
            return category::array;
        }
        if constexpr (std::is_same_v<T, callable_type>)
        {
            return category::callable;
        }
        if constexpr (std::is_same_v<T, lambda_type>)
        {
            return category::lambda;
        }
    }

    template <class T>
    void ensure_type() const
    {
        static_assert(is_valid_type<T>, "invalid type");

        if (is<T>())
        {
            return;
        }
        std::stringstream ss;
        ss << "accessing: " << to_category<T>() << ", actual: " << category();
        throw std::runtime_error{ ss.str() };
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
