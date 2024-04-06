#pragma once

#include <vector>

template <class Head, class... Tail>
auto vec(Head&& head, Tail&&... tail) -> std::vector<std::decay_t<Head>>
{
    std::vector<std::decay_t<Head>> result;
    result.push_back(std::forward<Head>(head));
    (result.push_back(std::forward<Tail>(tail)), ...);
    return result;
}

template <class T>
std::vector<T> concat(std::vector<T> lhs, const std::vector<T>& rhs)
{
    lhs.insert(lhs.end(), rhs.begin(), rhs.end());
    return lhs;
}

template <class T>
T pop_front(std::vector<T>& v)
{
    if (v.empty())
    {
        throw std::runtime_error{ "Cannot pop from empty vector" };
    }
    T result = v.front();
    v.erase(std::begin(v));
    return result;
}
