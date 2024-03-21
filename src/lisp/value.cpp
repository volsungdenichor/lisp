#include "lisp/value.hpp"

namespace lisp
{

template <class BinaryOp>
value op(const value& lhs, const value& rhs, BinaryOp op, std::string_view op_name)
{
    if (lhs.is<value::integer_type>() && rhs.is<value::integer_type>())
    {
        return op(lhs.as<value::integer_type>(), rhs.as<value::integer_type>());
    }
    else if (lhs.is<value::integer_type>() && rhs.is<value::floating_point_type>())
    {
        return op(lhs.as<value::integer_type>(), rhs.as<value::floating_point_type>());
    }
    else if (lhs.is<value::floating_point_type>() && rhs.is<value::integer_type>())
    {
        return op(lhs.as<value::floating_point_type>(), rhs.as<value::integer_type>());
    }
    throw std::runtime_error{ str("Cannot ", op_name, " ", lhs.type(), " and ", rhs.type()) };
}

template <class BinaryOp>
bool cmp(const value& lhs, const value& rhs, BinaryOp op)
{
    if (lhs.is<value::integer_type>() && rhs.is<value::integer_type>())
    {
        return op(lhs.as<value::integer_type>(), rhs.as<value::integer_type>());
    }
    else if (lhs.is<value::integer_type>() && rhs.is<value::floating_point_type>())
    {
        return op(lhs.as<value::integer_type>(), rhs.as<value::floating_point_type>());
    }
    else if (lhs.is<value::floating_point_type>() && rhs.is<value::integer_type>())
    {
        return op(lhs.as<value::floating_point_type>(), rhs.as<value::integer_type>());
    }
    else if (lhs.is<value::string_type>() && rhs.is<value::string_type>())
    {
        return op(lhs.as<value::string_type>(), rhs.as<value::string_type>());
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
    else if (lhs.is<value::null_type>())
    {
        return true;
    }
    else if (lhs.is<value::string_type>())
    {
        return lhs.as<value::string_type>() == rhs.as<value::string_type>();
    }
    else if (lhs.is<value::symbol_type>())
    {
        return lhs.as<value::symbol_type>() == rhs.as<value::symbol_type>();
    }
    else if (lhs.is<value::integer_type>())
    {
        return lhs.as<value::integer_type>() == rhs.as<value::integer_type>();
    }
    else if (lhs.is<value::floating_point_type>())
    {
        return lhs.as<value::floating_point_type>() == rhs.as<value::floating_point_type>();
    }
    else if (lhs.is<value::boolean_type>())
    {
        return lhs.as<value::boolean_type>() == rhs.as<value::boolean_type>();
    }
    else if (lhs.is<value::array_type>())
    {
        return lhs.as<value::array_type>() == rhs.as<value::array_type>();
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
