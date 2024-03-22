#include <iostream>
#include <lisp/lisp.hpp>

void run()
{
    lisp::stack_type stack = lisp::default_stack();
    using namespace lisp::literals;
    const lisp::value val = lisp::array{
        //
        "begin"_s,
        lisp::array{
            "let"_s, "func"_s, lisp::array{ "lambda"_s, lisp::array{ "arg"_s }, lisp::array{ "/"_s, "arg"_s, 10.0 } } },
        lisp::array{ "print"_s, lisp::array{ "list"_s, 1, 2, 3 } },
        lisp::array{ "print"_s, lisp::array{ "func"_s, 3 } },
    };

    std::cout << val << std::endl;
    std::cout << lisp::evaluate(val, stack) << std::endl;
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
