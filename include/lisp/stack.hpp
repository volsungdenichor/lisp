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
    frame_type frame;
    stack_base* outer;

    stack_base(frame_type frame, stack_base* outer = {}) : frame{ std::move(frame) }, outer{ outer }
    {
    }

    const value_type& insert(const symbol_type& s, const value_type& v)
    {
        frame[s] = v;
        return v;
    }

    const value_type& operator[](const symbol_type& s) const
    {
        const auto iter = frame.find(s);
        if (iter != frame.end())
        {
            return iter->second;
        }
        if (outer)
        {
            return (*outer)[s];
        }

        throw std::runtime_error{ str("Unrecognized symbol '", s, "'") };
    }
};
}  // namespace lisp