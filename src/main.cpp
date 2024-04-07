#include <fstream>
#include <iomanip>
#include <iostream>
#include <lisp/lisp.hpp>
#include <lisp/utils/ansi.hpp>
#include <lisp/utils/pipeline.hpp>
#include <lisp/utils/std_ostream.hpp>
#include <sstream>

struct load_file
{
    std::string operator()(const std::string& path) const
    {
        std::ifstream file{ path.c_str() };
        if (!file)
        {
            throw std::runtime_error{ str("Cannot read from ", path, ".") };
        }
        return { std::istreambuf_iterator<char>{ file }, std::istreambuf_iterator<char>{} };
    }
};

auto trim_front(const std::function<bool(char)>& pred)
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

auto trim_back(const std::function<bool(char)>& pred)
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

auto trim(const std::function<bool(char)>& pred)
{
    return fn(trim_front(pred)) |= fn(trim_back(pred));
}

auto starts_with(std::string prefix)
{
    return [=](std::string_view txt) { return txt.size() >= prefix.size() && txt.substr(0, prefix.size()) == prefix; };
}

struct split_lines
{
    std::vector<std::string> operator()(std::string text) const
    {
        std::vector<std::string> result;
        std::stringstream ss{ text };

        for (std::string line; std::getline(ss, line, '\n');)
        {
            result.push_back(line);
        }

        return result;
    }
};

struct filter_comments
{
    std::vector<std::string> operator()(std::vector<std::string> in) const
    {
        static const auto is_space = [](char ch) { return std::isspace(ch); };
        static const auto is_comment = fn(trim(is_space)) |= fn(starts_with(";"));

        in.erase(std::remove_if(in.begin(), in.end(), is_comment), in.end());
        return in;
    }
};

struct join_lines
{
    std::string operator()(const std::vector<std::string>& lines) const
    {
        std::stringstream ss;
        std::copy(lines.begin(), lines.end(), std::ostream_iterator<std::string>{ ss, "\n" });
        return ss.str();
    }
};

int run(int argc, char* argv[])
{
    lisp::stack_type stack = lisp::default_stack();

    const auto val = "src/input.lisp"  //
        |= fn(load_file{})             //
        |= fn(split_lines{})           //
        |= fn(filter_comments{})       //
        |= fn(join_lines{})            //
        |= fn([](const std::string& c) { return lisp::parse(c); });

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
