#pragma once

#include <lisp/utils/string_utils.hpp>
#include <map>
#include <vector>

namespace lisp
{

template <class S, class V>
struct stack_base
{
    using symbol_type = S;
    using value_type = V;
    using frame_type = std::map<symbol_type, value_type>;
    std::vector<frame_type> frames;

    stack_base(std::vector<frame_type> f) : frames{ std::move(f) }
    {
    }

    const value_type& insert(const symbol_type& s, const value_type& v)
    {
        frames.back()[s] = v;
        return v;
    }

    const value_type& operator[](const symbol_type& s) const
    {
        for (auto it = frames.rbegin(); it != frames.rend(); ++it)
        {
            const auto& f = *it;
            const auto iter = f.find(s);
            if (iter != f.end())
            {
                return iter->second;
            }
        }
        throw std::runtime_error{ str("Unrecognized symbol '", s, "'") };
    }

    stack_base next() const
    {
        stack_base result{ *this };
        result.frames.push_back(frame_type{});
        return result;
    }
};
}  // namespace lisp