#pragma once

#include <string>

namespace lisp
{

class symbol
{
private:
    std::string m_str;

public:
    explicit symbol(std::string str) : m_str{ std::move(str) }
    {
    }

    symbol(const symbol&) = default;
    symbol(symbol&&) = default;

    symbol& operator=(symbol other)
    {
        std::swap(m_str, other.m_str);
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const symbol& item)
    {
        return os << item.m_str;
    }

    friend bool operator==(const symbol& lhs, const symbol& rhs)
    {
        return lhs.m_str == rhs.m_str;
    }

    friend bool operator!=(const symbol& lhs, const symbol& rhs)
    {
        return lhs.m_str != rhs.m_str;
    }

    friend bool operator<(const symbol& lhs, const symbol& rhs)
    {
        return lhs.m_str < rhs.m_str;
    }

    friend bool operator<=(const symbol& lhs, const symbol& rhs)
    {
        return lhs.m_str <= rhs.m_str;
    }

    friend bool operator>(const symbol& lhs, const symbol& rhs)
    {
        return lhs.m_str > rhs.m_str;
    }

    friend bool operator>=(const symbol& lhs, const symbol& rhs)
    {
        return lhs.m_str >= rhs.m_str;
    }
};

namespace literals
{

auto operator""_s(const char* str, std::size_t size) -> symbol
{
    return symbol(std::string(str, size));
}

}  // namespace literals

}  // namespace lisp
