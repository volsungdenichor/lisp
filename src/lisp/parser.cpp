#include <lisp/parser.hpp>
#include <lisp/tokenizer.hpp>
#include <lisp/utils/container_utils.hpp>
#include <lisp/utils/iterator_range.hpp>
#include <optional>

namespace lisp
{

std::optional<value::null_type> as_null(const token& tok)
{
    if (tok == "null")
    {
        return null;
    }
    return {};
}

std::optional<value::string_type> as_string(const token& tok)
{
    if (tok.front() == '\"' && tok.back() == '\"')
    {
        return iterator_range{ tok }.drop(1).drop_back(1);
    }
    return {};
}

std::optional<value::integer_type> as_integer(const token& tok)
{
    if (std::all_of(std::begin(tok), std::end(tok), [](char ch) { return std::isdigit(ch); }))
    {
        return value::integer_type{ std::atoi(tok.c_str()) };
    }
    return {};
}

std::optional<value::floating_point_type> as_floating_point(const token& tok)
{
    std::stringstream ss;
    ss << tok;
    value::floating_point_type res;
    ss >> res;
    if (ss)
    {
        return res;
    }
    return {};
}

std::optional<value::boolean_type> as_boolean(const token& tok)
{
    if (tok == "true")
    {
        return value::boolean_type{ true };
    }
    if (tok == "false")
    {
        return value::boolean_type{ false };
    }
    return {};
}

std::optional<value::symbol_type> as_symbol(const token& tok)
{
    return value::symbol_type{ tok };
}

value read_atom(const token& tok)
{
    if (const auto v = as_string(tok))
    {
        return *v;
    }
    else if (const auto v = as_integer(tok))
    {
        return *v;
    }
    else if (const auto v = as_floating_point(tok))
    {
        return *v;
    }
    else if (const auto v = as_boolean(tok))
    {
        return *v;
    }
    else if (const auto v = as_null(tok))
    {
        return *v;
    }
    else if (const auto v = as_symbol(tok))
    {
        return *v;
    }
    throw std::runtime_error{ str("Unrecognized token '", tok, "'") };
}

value read_from(std::vector<token>& tokens)
{
    if (tokens.empty())
    {
        return null;
    }
    const auto front = pop_front(tokens);
    if (front == "'")
    {
        return array{ symbol{ "quote" }, read_from(tokens) };
    }
    if (front == "(")
    {
        auto result = array{};
        if (tokens.empty())
        {
            throw std::runtime_error{ "Invalid parentheses " };
        }
        while (!tokens.empty() && tokens.front() != ")")
        {
            result.push_back(read_from(tokens));
        }
        pop_front(tokens);
        return result;
    }
    else
    {
        return read_atom(front);
    }
}

value parse(std::string_view text)
{
    auto tokens = tokenize(text);
    return read_from(tokens);
}

}  // namespace lisp
