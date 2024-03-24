#include <algorithm>
#include <cassert>
#include <lisp/tokenizer.hpp>
#include <lisp/utils/string_utils.hpp>
#include <optional>
#include <tuple>

namespace lisp
{

using tokenizer_result = std::tuple<token, std::string_view>;

tokenizer_result read_quoted_string(std::string_view text)
{
    assert(!text.empty());
    auto it = std::begin(text) + 1;
    token result = "\"";
    while (it != std::end(text))
    {
        if (it[0] == '\\' && std::distance(it, std::end(text)) > 1 && it[1] == '"')
        {
            result += '"';
            it += 2;
        }
        else if (it[0] == '"')
        {
            result += *it++;
            break;
        }
        else
        {
            result += *it++;
        }
    }
    return tokenizer_result{ result, make_string_view(it, std::end(text)) };
}

std::optional<tokenizer_result> read_token(std::string_view text)
{
    static const auto is_parenthesis = [](char ch) { return ch == '(' || ch == ')'; };
    static const auto is_quotation_mark = [](char ch) { return ch == '"'; };
    static const auto is_space = [](char ch) { return std::isspace(ch); };

    if (text.empty())
    {
        return {};
    }
    if (is_parenthesis(text[0]))
    {
        return std::tuple{ std::string(1, text[0]), text.substr(1) };
    }
    if (is_quotation_mark(text[0]))
    {
        return read_quoted_string(text);
    }

    const auto b = std::begin(text);
    const auto e = std::end(text);

    const auto iter = std::find_if(b, e, [](char ch) { return is_space(ch) || is_parenthesis(ch); });
    if (iter != e)
    {
        if (is_space(*iter))
        {
            return tokenizer_result{ token{ b, iter }, make_string_view(iter + 1, e) };
        }
        else
        {
            return tokenizer_result{ token{ b, iter }, make_string_view(iter, e) };
        }
    }
    else
    {
        return tokenizer_result{ token{ std::move(text) }, std::string_view{} };
    }
}

std::vector<token> tokenize(std::string_view text)
{
    std::vector<token> result;
    while (true)
    {
        const auto res = read_token(text);
        if (!res)
        {
            break;
        }
        auto [token, remainder] = *res;
        if (!token.empty())
        {
            result.push_back(std::move(token));
        }
        text = std::move(remainder);
    }
    return result;
}

}  // namespace lisp
