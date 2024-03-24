#include <lisp/evaluate.hpp>

namespace lisp
{

value evaluate(const value& expr, stack_type& stack)
{
    static const auto is = [](const value& v, const value::string_type& s) { return v == value{ value::symbol_type(s) }; };
    if (expr.is_array())
    {
        const auto& a = expr.as_array();
        if (a.size() == 4)
        {
            if (is(a.front(), "if"))
            {
                return evaluate(a.at(1), stack).as_boolean() ? evaluate(a.at(2), stack) : evaluate(a.at(3), stack);
            }
        }
        else if (a.size() == 3)
        {
            if (is(a.front(), "let"))
            {
                auto res = evaluate(a.at(2), stack);
                return stack.insert(a.at(1).as_symbol(), res);
            }
            else if (is(a.front(), "lambda"))
            {
                return value::lambda_type{ a.at(1), a.at(2), stack };
            }
        }
        else if (a.size() == 2)
        {
            if (is(a.front(), "quote") || is(a.front(), "'"))
            {
                return a.at(1);
            }
        }
        if (is(a.front(), "begin"))
        {
            value result = {};
            for (auto it = std::next(std::begin(a)); it != std::end(a); ++it)
            {
                result = evaluate(*it, stack);
            }
            return result;
        }

        const std::vector<value> arg_values = std::invoke(
            [&]()
            {
                std::vector<value> result;
                result.reserve(a.size() - 1);
                std::transform(
                    std::begin(a) + 1,
                    std::end(a),
                    std::back_inserter(result),
                    [&](const value& v) { return evaluate(v, stack); });
                return result;
            });

        const auto func = evaluate(a.at(0), stack);
        if (func.is_callable())
        {
            return func.as_callable()(arg_values);
        }
        else if (func.is_lambda())
        {
            const auto& lambda = func.as_lambda();
            const auto& params = lambda.params.as_array();
            if (params.size() != arg_values.size())
            {
                throw std::runtime_error{ str(
                    "Invalid argument number: expected ", params.size(), ", got ", arg_values.size()) };
            }
            auto new_stack = lambda.stack.next();
            for (std::size_t i = 0; i < params.size(); ++i)
            {
                new_stack.insert(params.at(i).as_symbol(), arg_values.at(i));
            }

            const auto fn = [&](const std::vector<value>& arguments) -> value { return evaluate(lambda.body, new_stack); };
            return value::callable_type{ fn, "lambda", params.size() }(arg_values);
        }
        else
        {
            throw std::runtime_error{ str("Error on executing ", func) };
        }
    }
    else if (expr.is_symbol())
    {
        return stack[expr.as_symbol()];
    }
    return expr;
}

}  // namespace lisp