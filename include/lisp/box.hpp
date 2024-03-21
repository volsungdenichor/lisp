#pragma once

#include <iostream>
#include <memory>

template <class T>
class box
{
public:
    using value_type = T;

    template <class U, std::enable_if_t<std::is_constructible_v<value_type, U>, int> = 0>
    box(U&& value) : m_ptr(std::make_unique<T>(std::move(value)))
    {
    }

    box(const box& other) : box(*other)
    {
    }

    box(box&&) = default;

    box& operator=(box other)
    {
        std::swap(m_ptr, other.m_ptr);
        return *this;
    }

    template <class U, std::enable_if_t<std::is_constructible_v<value_type, U>, int> = 0>
    box& operator=(U&& value)
    {
        return *this = box{ std::forward<U>(value) };
    }

    value_type& operator*()
    {
        return *m_ptr;
    }

    const value_type& operator*() const
    {
        return *m_ptr;
    }

    friend std::ostream& operator<<(std::ostream& os, const box& item)
    {
        return os << *item;
    }

private:
    std::unique_ptr<value_type> m_ptr;
};
