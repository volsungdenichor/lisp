#pragma once

#include <lisp/functions.hpp>
#include <lisp/value.hpp>

namespace lisp
{

inline stack_type::frame_type default_frame()
{
    using namespace literals;
    return stack_type::frame_type{
        { "print"_s, callable{ print{}, "print" } },
        { "+"_s, callable{ binary{ std::plus{} }, "plus", 2 } },
        { "-"_s, callable{ binary{ std::minus{} }, "minus", 2 } },
        { "*"_s, callable{ binary{ std::multiplies{} }, "multiplies", 2 } },
        { "/"_s, callable{ binary{ std::divides{} }, "divides", 2 } },
        { "=="_s, callable{ binary{ std::equal_to{} }, "equal_to", 2 } },
        { "!="_s, callable{ binary{ std::not_equal_to{} }, "not_equal_to", 2 } },
        { "<"_s, callable{ binary{ std::less{} }, "less", 2 } },
        { "<="_s, callable{ binary{ std::less_equal{} }, "less_equal", 2 } },
        { ">"_s, callable{ binary{ std::greater{} }, "greater", 2 } },
        { ">="_s, callable{ binary{ std::greater_equal{} }, "greater_equal", 2 } },
        { "car"_s, callable{ car{}, "car", 1 } },
        { "cdr"_s, callable{ cdr{}, "cdr", 1 } },
        { "cons"_s, callable{ cons{}, "cons", 2 } },
        { "list"_s, callable{ list{}, "list" } },
    };
}

inline stack_type default_stack()
{
    return { { default_frame() } };
}

}  // namespace lisp
