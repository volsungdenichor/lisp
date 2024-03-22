#include "lisp/value.hpp"

#include <iomanip>

namespace lisp
{

value::value() : m_data{ null_type{} }
{
}

value& value::operator=(value other)
{
    m_data.~variant_type();
    new (&m_data) variant_type{ std::move(other.m_data) };
    return *this;
}

category value::type() const
{
    return std::visit(
        overload{ [](const null_type&) { return category::null; },
                  [](const string_type&) { return category::string; },
                  [](const symbol_type&) { return category::symbol; },
                  [](const integer_type&) { return category::integer; },
                  [](const floating_point_type&) { return category::floating_point; },
                  [](const boolean_type&) { return category::boolean; },
                  [](const array_type&) { return category::array; },
                  [](const callable_type&) { return category::callable; },
                  [](const box<lambda_type>&) { return category::lambda; } },
        m_data);
}

bool value::is_null() const
{
    return std::holds_alternative<null_type>(m_data);
}

bool value::is_string() const
{
    return std::holds_alternative<string_type>(m_data);
}

bool value::is_symbol() const
{
    return std::holds_alternative<symbol_type>(m_data);
}

bool value::is_integer() const
{
    return std::holds_alternative<integer_type>(m_data);
}

bool value::is_boolean() const
{
    return std::holds_alternative<boolean_type>(m_data);
}

bool value::is_floating_point() const
{
    return std::holds_alternative<floating_point_type>(m_data);
}

bool value::is_array() const
{
    return std::holds_alternative<array_type>(m_data);
}

bool value::is_callable() const
{
    return std::holds_alternative<callable_type>(m_data);
}

bool value::is_lambda() const
{
    return std::holds_alternative<box<lambda_type>>(m_data);
}

const value::null_type& value::as_null() const
{
    const auto ptr = std::get_if<null_type>(&m_data);
    if (!ptr)
    {
        throw std::runtime_error{ str("accessing: ", category_type::null, ", actual: ", category()) };
    }
    return *ptr;
}

const value::string_type& value::as_string() const
{
    const auto ptr = std::get_if<string_type>(&m_data);
    if (!ptr)
    {
        throw std::runtime_error{ str("accessing: ", category_type::string, ", actual: ", category()) };
    }
    return *ptr;
}

const value::symbol_type& value::as_symbol() const
{
    const auto ptr = std::get_if<symbol_type>(&m_data);
    if (!ptr)
    {
        throw std::runtime_error{ str("accessing: ", category_type::symbol, ", actual: ", category()) };
    }
    return *ptr;
}

const value::integer_type& value::as_integer() const
{
    const auto ptr = std::get_if<integer_type>(&m_data);
    if (!ptr)
    {
        throw std::runtime_error{ str("accessing: ", category_type::integer, ", actual: ", category()) };
    }
    return *ptr;
}

const value::boolean_type& value::as_boolean() const
{
    const auto ptr = std::get_if<boolean_type>(&m_data);
    if (!ptr)
    {
        throw std::runtime_error{ str("accessing: ", category_type::boolean, ", actual: ", category()) };
    }
    return *ptr;
}

const value::floating_point_type& value::as_floating_point() const
{
    const auto ptr = std::get_if<floating_point_type>(&m_data);
    if (!ptr)
    {
        throw std::runtime_error{ str("accessing: ", category_type::floating_point, ", actual: ", category()) };
    }
    return *ptr;
}

const value::array_type& value::as_array() const
{
    const auto ptr = std::get_if<array_type>(&m_data);
    if (!ptr)
    {
        throw std::runtime_error{ str("accessing: ", category_type::array, ", actual: ", category()) };
    }
    return *ptr;
}

const value::callable_type& value::as_callable() const
{
    const auto ptr = std::get_if<callable_type>(&m_data);
    if (!ptr)
    {
        throw std::runtime_error{ str("accessing: ", category_type::callable, ", actual: ", category()) };
    }
    return *ptr;
}

const value::lambda_type& value::as_lambda() const
{
    const auto ptr = std::get_if<box<lambda_type>>(&m_data);
    if (!ptr)
    {
        throw std::runtime_error{ str("accessing: ", category_type::lambda, ", actual: ", category()) };
    }
    return **ptr;
}

std::ostream& operator<<(std::ostream& os, const value& item)
{
    std::visit(
        overload{ [&](const value::null_type& v) { os << "null"; },
                  [&](const value::string_type& v) { os << std::quoted(v); },
                  [&](const value::symbol_type& v) { os << v; },
                  [&](const value::integer_type& v) { os << v; },
                  [&](const value::floating_point_type& v) { os << std::fixed << std::setprecision(1) << v; },
                  [&](const value::boolean_type& v) { os << std::boolalpha << v; },
                  [&](const value::array_type& v) { os << "(" << delimit(v, " ") << ")"; },
                  [&](const value::callable_type& v) { os << v.name; },
                  [&](const box<value::lambda_type>& v) { os << "lambda " << (*v).params; } },
        item.m_data);
    return os;
}

template <class BinaryOp>
value op(const value& lhs, const value& rhs, BinaryOp op, std::string_view op_name)
{
    if (lhs.is_integer() && rhs.is_integer())
    {
        return op(lhs.as_integer(), rhs.as_integer());
    }
    else if (lhs.is_integer() && rhs.is_floating_point())
    {
        return op(lhs.as_integer(), rhs.as_floating_point());
    }
    else if (lhs.is_floating_point() && rhs.is_integer())
    {
        return op(lhs.as_floating_point(), rhs.as_integer());
    }
    throw std::runtime_error{ str("Cannot ", op_name, " ", lhs.type(), " and ", rhs.type()) };
}

template <class BinaryOp>
bool cmp(const value& lhs, const value& rhs, BinaryOp op)
{
    if (lhs.is_integer() && rhs.is_integer())
    {
        return op(lhs.as_integer(), rhs.as_integer());
    }
    else if (lhs.is_integer() && rhs.is_floating_point())
    {
        return op(lhs.as_integer(), rhs.as_floating_point());
    }
    else if (lhs.is_floating_point() && rhs.is_integer())
    {
        return op(lhs.as_floating_point(), rhs.as_integer());
    }
    else if (lhs.is_string() && rhs.is_string())
    {
        return op(lhs.as_string(), rhs.as_string());
    }
    throw std::runtime_error{ str("Cannot compare ", lhs.type(), " and ", rhs.type()) };
}

value operator+(const value& lhs, const value& rhs)
{
    return op(lhs, rhs, std::plus{}, "add");
}

value operator-(const value& lhs, const value& rhs)
{
    return op(lhs, rhs, std::minus{}, "subtract");
}

value operator*(const value& lhs, const value& rhs)
{
    return op(lhs, rhs, std::multiplies{}, "multiply");
}

value operator/(const value& lhs, const value& rhs)
{
    return op(lhs, rhs, std::divides{}, "divide");
}

bool operator==(const value& lhs, const value& rhs)
{
    if (lhs.type() != rhs.type())
    {
        return false;
    }
    else if (lhs.is_null())
    {
        return true;
    }
    else if (lhs.is_string())
    {
        return lhs.as_string() == rhs.as_string();
    }
    else if (lhs.is_symbol())
    {
        return lhs.as_symbol() == rhs.as_symbol();
    }
    else if (lhs.is_integer())
    {
        return lhs.as_integer() == rhs.as_integer();
    }
    else if (lhs.is_floating_point())
    {
        return lhs.as_floating_point() == rhs.as_floating_point();
    }
    else if (lhs.is_boolean())
    {
        return lhs.as_boolean() == rhs.as_boolean();
    }
    else if (lhs.is_array())
    {
        return lhs.as_array() == rhs.as_array();
    }

    return false;
}

bool operator!=(const value& lhs, const value& rhs)
{
    return !(lhs == rhs);
}

bool operator<(const value& lhs, const value& rhs)
{
    return cmp(lhs, rhs, std::less{});
}

bool operator<=(const value& lhs, const value& rhs)
{
    return cmp(lhs, rhs, std::less_equal{});
}

bool operator>(const value& lhs, const value& rhs)
{
    return cmp(lhs, rhs, std::greater{});
}

bool operator>=(const value& lhs, const value& rhs)
{
    return cmp(lhs, rhs, std::greater_equal{});
}
}  // namespace lisp
