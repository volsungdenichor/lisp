#include <fstream>
#include <iomanip>
#include <iostream>
#include <lisp/lisp.hpp>
#include <lisp/utils/ansi.hpp>
#include <lisp/utils/std_ostream.hpp>
#include <sstream>

std::string load_file(const std::string& path)
{
    std::ifstream file{ path.c_str() };
    if (!file)
    {
        throw std::runtime_error{ str("Cannot read from ", path, ".") };
    }
    return { std::istreambuf_iterator<char>{ file }, std::istreambuf_iterator<char>{} };
}

struct trim_fn
{
    static std::string_view trim_front(std::string_view txt, const std::function<bool(char)>& pred)
    {
        while (!txt.empty() && pred(txt.front()))
        {
            txt.remove_prefix(1);
        }
        return txt;
    }

    static std::string_view trim_back(std::string_view txt, const std::function<bool(char)>& pred)
    {
        while (!txt.empty() && pred(txt.back()))
        {
            txt.remove_suffix(1);
        }

        return txt;
    }

    std::string_view operator()(std::string_view txt, const std::function<bool(char)>& pred) const
    {
        return trim_back(trim_front(txt, pred), pred);
    }
};

auto trim(const std::function<bool(char)>& pred)
{
    return [=](std::string_view txt) { return trim_fn{}(txt, pred); };
}

auto starts_with(std::string prefix)
{
    return [=](std::string_view txt) { return txt.size() >= prefix.size() && txt.substr(0, prefix.size()) == prefix; };
}

std::vector<std::string> split(std::string text)
{
    std::vector<std::string> result;
    std::stringstream ss{ text };

    for (std::string line; std::getline(ss, line, '\n');)
    {
        result.push_back(line);
    }

    return result;
}

std::vector<std::string> filter_comments(std::vector<std::string> in)
{
    static const auto is_space = [](char ch) { return std::isspace(ch); };
    static const auto is_comment = [](std::string_view line) { return starts_with(";")(trim(is_space)(line)); };
    in.erase(std::remove_if(in.begin(), in.end(), is_comment), in.end());
    return in;
}

std::string join(const std::vector<std::string>& lines)
{
    std::stringstream ss;
    std::copy(lines.begin(), lines.end(), std::ostream_iterator<std::string>{ ss, "\n" });
    return ss.str();
}

int run(int argc, char* argv[])
{
    const auto file_content = load_file("src/input.lisp");
    const auto content = join(filter_comments(split(file_content)));
    const auto val = lisp::parse(content);

    lisp::stack_type stack = lisp::default_stack();

    std::cout << lisp::evaluate(val, &stack) << std::endl;

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
