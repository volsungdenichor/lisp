#pragma once

#include <lisp/functions.hpp>
#include <lisp/value.hpp>

namespace lisp
{

stack_type::frame_type default_stack()
{
    using namespace literals;
    return stack_type::frame_type{
        { "print"_s, callable{ print{}, "print" } },
        { "+"_s, callable{ binary{ std::plus{} }, "plus" } },
        { "-"_s, callable{ binary{ std::minus{} }, "minus" } },
        { "*"_s, callable{ binary{ std::multiplies{} }, "multiplies" } },
        { "/"_s, callable{ binary{ std::divides{} }, "divides" } },
        { "=="_s, callable{ binary{ std::equal_to{} }, "equal_to" } },
        { "!="_s, callable{ binary{ std::not_equal_to{} }, "not_equal_to" } },
        { "<"_s, callable{ binary{ std::less{} }, "less" } },
        { "<="_s, callable{ binary{ std::less_equal{} }, "less_equal" } },
        { ">"_s, callable{ binary{ std::greater{} }, "greater" } },
        { ">="_s, callable{ binary{ std::greater_equal{} }, "greater_equal" } },
        { "car"_s, callable{ car{}, "car" } },
        { "cdr"_s, callable{ cdr{}, "cdr" } },
        { "cons"_s, callable{ cons{}, "cons" } },
        { "list"_s, callable{ list{}, "list" } },
    };
}

}  // namespace lisp
