#pragma once

#include <ostream>

namespace lisp
{

enum class category
{
    null,
    string,
    symbol,
    integer,
    boolean,
    floating_point,
    array,
    callable,
    lambda,
};

std::ostream& operator<<(std::ostream& os, const category item);

}  // namespace lisp
