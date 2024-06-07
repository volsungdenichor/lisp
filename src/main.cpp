#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <lisp/lisp.hpp>
#include <lisp/utils/ansi.hpp>
#include <lisp/utils/pipeline.hpp>
#include <lisp/utils/std_ostream.hpp>
#include <sstream>

template <class... Args>
using action_t = std::function<void(Args...)>;

template <class... Args>
using predicate_t = std::function<bool(Args...)>;

auto load_file(const std::string& path) -> std::string
{
    std::ifstream file{ path.c_str() };
    if (!file)
    {
        throw std::runtime_error{ str("Cannot read from ", path, ".") };
    }
    return { std::istreambuf_iterator<char>{ file }, std::istreambuf_iterator<char>{} };
}

auto trim_front(const predicate_t<char>& pred) -> std::function<std::string_view(std::string_view)>
{
    return [=](std::string_view txt)
    {
        while (!txt.empty() && pred(txt.front()))
        {
            txt.remove_prefix(1);
        }
        return txt;
    };
}

auto trim_back(const predicate_t<char>& pred) -> std::function<std::string_view(std::string_view)>
{
    return [=](std::string_view txt)
    {
        while (!txt.empty() && pred(txt.back()))
        {
            txt.remove_suffix(1);
        }
        return txt;
    };
}

auto trim(const predicate_t<char>& pred) -> std::function<std::string_view(std::string_view)>
{
    return fn(trim_front(pred)) |= fn(trim_back(pred));
}

auto starts_with(std::string prefix) -> predicate_t<std::string_view>
{
    return [=](std::string_view txt) { return txt.size() >= prefix.size() && txt.substr(0, prefix.size()) == prefix; };
}

auto split_lines(const std::string& text) -> std::vector<std::string>
{
    std::vector<std::string> result;
    std::stringstream ss{ text };

    for (std::string line; std::getline(ss, line, '\n');)
    {
        result.push_back(line);
    }

    return result;
}

auto filter_comments(std::vector<std::string> in) -> std::vector<std::string>
{
    static const auto is_space = [](char ch) { return std::isspace(ch); };
    static const auto is_comment = fn(trim(is_space)) |= fn(starts_with(";"));

    remove_erase_if(in, is_comment);
    return in;
}

auto join_lines(const std::vector<std::string>& lines) -> std::string
{
    std::stringstream ss;
    std::copy(lines.begin(), lines.end(), std::ostream_iterator<std::string>{ ss, "\n" });
    return ss.str();
}

int run(int argc, char* argv[])
{
    lisp::stack_type stack = lisp::default_stack();

    const auto file_name = argc >= 2  //
                               ? std::string{ argv[1] }
                               : std::string{ "../src/input.lisp" };

    const auto val = file_name   //
        |= fn(&load_file)        //
        |= fn(&split_lines)      //
        |= fn(&filter_comments)  //
        |= fn(&join_lines)       //
        |= fn(&lisp::parse);

    std::cout << ansi::fg(ansi::color::dark_blue) << val << ansi::reset << "\n";

    std::cout << ansi::fg(ansi::color::yellow);
    const auto result = lisp::evaluate(val, &stack);
    std::cout << ansi::reset;

    std::cout << ansi::fg(ansi::color::dark_green) << result << ansi::reset << "\n";

    // print_stack(stack);
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
        std::cerr << ansi::fg(ansi::color::red) << ex.what() << ansi::reset << '\n';
    }
}
