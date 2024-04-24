#include "lisp/value.hpp"

#include <iomanip>

#include "lisp/utils/type_traits.hpp"

namespace lisp
{

namespace
{

template <class T>
category to_category()
{
    if constexpr (std::is_same_v<T, value::null_type>)
    {
        return category::null;
    }
    else if constexpr (std::is_same_v<T, value::string_type>)
    {
        return category::string;
    }
    else if constexpr (std::is_same_v<T, value::symbol_type>)
    {
        return category::symbol;
    }
    else if constexpr (std::is_same_v<T, value::integer_type>)
    {
        return category::integer;
    }
    else if constexpr (std::is_same_v<T, value::floating_point_type>)
    {
        return category::floating_point;
    }
    else if constexpr (std::is_same_v<T, value::boolean_type>)
    {
        return category::boolean;
    }
    else if constexpr (std::is_same_v<T, value::array_type>)
    {
        return category::array;
    }
    else if constexpr (std::is_same_v<T, value::callable_type>)
    {
        return category::callable;
    }
    else if constexpr (std::is_same_v<T, box<value::lambda_type>>)
    {
        return category::lambda;
    }
    else
    {
        static_assert(always_false<T>(), "Type not handled");
    }
};

std::string build_message(category expected, category actual)
{
    return str("accessing: ", expected, ", actual: ", actual);
};

template <class T, class Variant>
const T& get_value(const Variant& variant, category actual)
{
    const auto ptr = std::get_if<T>(&variant);
    if (!ptr)
    {
        throw std::runtime_error{ build_message(to_category<T>(), actual) };
    }
    return *ptr;
}

}  // namespace

value::value() : m_data{ null_type{} }
{
}

value::value(null_type v) : m_data{ std::move(v) }
{
}

value::value(string_type v) : m_data{ std::move(v) }
{
}

value::value(symbol_type v) : m_data{ std::move(v) }
{
}

value::value(integer_type v) : m_data{ std::move(v) }
{
}

value::value(floating_point_type v) : m_data{ std::move(v) }
{
}

value::value(boolean_type v) : m_data{ std::move(v) }
{
}

value::value(array_type v) : m_data{ std::move(v) }
{
}

value::value(callable_type v) : m_data{ std::move(v) }
{
}

value::value(lambda_type v) : m_data{ std::move(v) }
{
}

value& value::operator=(const value& other)
{
    std::visit(
        overload{ [&](const value::null_type v) { this->m_data.emplace<null_type>(v); },
                  [&](const value::string_type& v) { this->m_data.emplace<string_type>(v); },
                  [&](const value::symbol_type& v) { this->m_data.emplace<symbol_type>(v); },
                  [&](const value::integer_type& v) { this->m_data.emplace<integer_type>(v); },
                  [&](const value::floating_point_type& v) { this->m_data.emplace<floating_point_type>(v); },
                  [&](const value::boolean_type& v) { this->m_data.emplace<boolean_type>(v); },
                  [&](const value::array_type& v) { this->m_data.emplace<array_type>(v); },
                  [&](const value::callable_type& v) { this->m_data.emplace<callable_type>(v); },
                  [&](const box<value::lambda_type>& v) { this->m_data.emplace<box<lambda_type>>(v); } },
        other.m_data);
    return *this;
}

value::operator bool() const
{
    return is_boolean() && as_boolean();
}

category value::get_category() const
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
    return get_value<value::null_type>(m_data, get_category());
}

const value::string_type& value::as_string() const
{
    return get_value<value::string_type>(m_data, get_category());
}

const value::symbol_type& value::as_symbol() const
{
    return get_value<value::symbol_type>(m_data, get_category());
}

const value::integer_type& value::as_integer() const
{
    return get_value<value::integer_type>(m_data, get_category());
}

const value::boolean_type& value::as_boolean() const
{
    return get_value<value::boolean_type>(m_data, get_category());
}

const value::floating_point_type& value::as_floating_point() const
{
    return get_value<value::floating_point_type>(m_data, get_category());
}

const value::array_type& value::as_array() const
{
    return get_value<value::array_type>(m_data, get_category());
}

const value::callable_type& value::as_callable() const
{
    return get_value<value::callable_type>(m_data, get_category());
}

const value::lambda_type& value::as_lambda() const
{
    return *get_value<box<lambda_type>>(m_data, get_category());
}

std::ostream& operator<<(std::ostream& os, const value& item)
{
    std::visit(
        overload{ [&](const value::null_type& v) { os << "null"; },
                  [&](const value::string_type& v) { os << v; },
                  [&](const value::symbol_type& v) { os << v; },
                  [&](const value::integer_type& v) { os << v; },
                  [&](const value::floating_point_type& v) { os << std::fixed << std::setprecision(1) << v; },
                  [&](const value::boolean_type& v) { os << std::boolalpha << v; },
                  [&](const value::array_type& v) { os << "(" << delimit(v, " ") << ")"; },
                  [&](const value::callable_type& v)
                  {
                      os << v.name;
                      if (!v.bound_args.empty())
                      {
                          os << ", bound_args=[" << delimit(v.bound_args, ", ") << "]";
                      }
                  },
                  [&](const box<value::lambda_type>& v) { os << "lambda " << (*v).params << " " << (*v).body; } },
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
    throw std::runtime_error{ str("Cannot ", op_name, " ", lhs.get_category(), " and ", rhs.get_category()) };
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
    throw std::runtime_error{ str("Cannot compare ", lhs.get_category(), " and ", rhs.get_category()) };
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

value operator%(const value& lhs, const value& rhs)
{
    if (lhs.is_integer() && rhs.is_integer())
    {
        return lhs.as_integer() % rhs.as_integer();
    }
    throw std::runtime_error{ str("Cannot mod ", lhs.get_category(), " and ", rhs.get_category()) };
}

bool operator==(const value& lhs, const value& rhs)
{
    if (lhs.get_category() != rhs.get_category())
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
