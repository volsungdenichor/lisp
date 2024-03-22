#include <iostream>
#include <lisp/lisp.hpp>
#include <lisp/utils/std_ostream.hpp>

void run()
{
    std::string text = R"(
        (begin
            (let val 123)
            (let func (lambda (arg) (/ arg 10.0)))
            (print val)
            (print (func val))
        )
    )";
    const auto val = lisp::parse(text);

    lisp::stack_type stack = lisp::default_stack();

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
