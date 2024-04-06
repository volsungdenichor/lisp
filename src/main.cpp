#include <fstream>
#include <iostream>
#include <lisp/lisp.hpp>
#include <lisp/utils/std_ostream.hpp>

std::string load_file(const std::string& path)
{
    std::ifstream file{ path.c_str() };
    if (!file)
    {
        throw std::runtime_error{ str("Cannot read from ", path, ".") };
    }
    return std::string{ (std::istreambuf_iterator<char>{ file }), std::istreambuf_iterator<char>{} };
}

int run(int argc, char* argv[])
{
    const std::string file_content = load_file("src/input.lisp");
    const auto val = lisp::parse(file_content);

    lisp::stack_type stack = lisp::default_stack();

    // std::cout << val << std::endl;

    std::cout << lisp::evaluate(val, &stack) << std::endl;
    return 0;
}

int main(int argc, char* argv[])
{
    try
    {
        return run(argc, argv);
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << '\n';
    }
}
