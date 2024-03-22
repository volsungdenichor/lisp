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
    return match(
        [](const null_type&) { return category::null; },
        [](const string_type&) { return category::string; },
        [](const symbol_type&) { return category::symbol; },
        [](const integer_type&) { return category::integer; },
        [](const floating_point_type&) { return category::floating_point; },
        [](const boolean_type&) { return category::boolean; },
        [](const array_type&) { return category::array; },
        [](const callable_type&) { return category::callable; },
        [](const lambda_type&) { return category::lambda; });
}

bool value::is_null() const
{
    return is<null_type>();
}

bool value::is_string() const
{
    return is<string_type>();
}

bool value::is_symbol() const
{
    return is<symbol_type>();
}

bool value::is_integer() const
{
    return is<integer_type>();
}

bool value::is_boolean() const
{
    return is<boolean_type>();
}

bool value::is_floating_point() const
{
    return is<floating_point_type>();
}

bool value::is_array() const
{
    return is<array_type>();
}

bool value::is_callable() const
{
    return is<callable_type>();
}

bool value::is_lambda() const
{
    return is<lambda_type>();
}

const value::null_type& value::as_null() const
{
    return as<null_type>();
}

const value::string_type& value::as_string() const
{
    return as<string_type>();
}

const value::symbol_type& value::as_symbol() const
{
    return as<symbol_type>();
}

const value::integer_type& value::as_integer() const
{
    return as<integer_type>();
}

const value::boolean_type& value::as_boolean() const
{
    return as<boolean_type>();
}

const value::floating_point_type& value::as_floating_point() const
{
    return as<floating_point_type>();
}

const value::array_type& value::as_array() const
{
    return as<array_type>();
}

const value::callable_type& value::as_callable() const
{
    return as<callable_type>();
}

const value::lambda_type& value::as_lambda() const
{
    return as<lambda_type>();
}

std::ostream& operator<<(std::ostream& os, const value& item)
{
    item.match(
        [&](const value::null_type& v) { os << "null"; },
        [&](const value::string_type& v) { os << std::quoted(v); },
        [&](const value::symbol_type& v) { os << v; },
        [&](const value::integer_type& v) { os << v; },
        [&](const value::floating_point_type& v) { os << std::fixed << std::setprecision(1) << v; },
        [&](const value::boolean_type& v) { os << std::boolalpha << v; },
        [&](const value::array_type& v) { os << "(" << delimit(v, " ") << ")"; },
        [&](const value::callable_type& v) { os << v.name; },
        [&](const value::lambda_type& v) { os << "lambda " << (*v).params; });
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
