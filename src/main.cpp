#include <iostream>
#include <lisp/lisp.hpp>

void run()
{
    lisp::stack_type s{ { lisp::default_stack() } };
    using namespace lisp::literals;
    const lisp::value v = lisp::array{
        //
        "begin"_s,
        lisp::array{ "print"_s, lisp::array{ "list"_s, 1, 2, 3 } },
        lisp::array{ "print"_s, lisp::array{ "quote"_s, lisp::array{ 1, 2, 3 } } },
    };

    std::cout << v << std::endl;
    std::cout << lisp::evaluate(v, s) << std::endl;
}

int main()
{
    try
    {
        run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << '\n';
    }
}
