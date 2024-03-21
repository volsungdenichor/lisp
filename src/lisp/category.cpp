#include <lisp/category.hpp>

namespace lisp
{

std::ostream& operator<<(std::ostream& os, const category item)
{
#define CASE(x) \
    case category::x: return os << #x
    switch (item)
    {
        CASE(null);
        CASE(string);
        CASE(symbol);
        CASE(integer);
        CASE(boolean);
        CASE(floating_point);
        CASE(array);
        CASE(callable);
        CASE(lambda);
        default: throw std::runtime_error{ "invalid value_category" };
    }
    return os;
}

}  // namespace lisp
