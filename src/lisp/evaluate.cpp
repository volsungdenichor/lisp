#include <lisp/evaluate.hpp>
#include <lisp/utils/iterator_range.hpp>

namespace lisp
{

array do_apply_macro(const array& a)
{
    if (a.size() == 4 && a.at(0) == symbol{ "defun" })
    {
        return array{ symbol{ "let" }, a.at(1), array{ symbol{ "lambda" }, a.at(2), a.at(3) } };
    }
    if (a.size() > 2 && a.at(1) == symbol{ "|>" })
    {
        return array{ array{ std::begin(a) + 2, std::end(a) }, a.at(0) };
    }
    return a;
}

array apply_macro(const array& a)
{
    array result = do_apply_macro(a);
    return result;
}

value evaluate(const value& expr, stack_type* stack)
{
    static const auto is = [](const value& v, const value::string_type& s) { return v == value{ value::symbol_type(s) }; };
    if (expr.is_array())
    {
        const auto a = apply_macro(expr.as_array());
        const auto args = iterator_range{ a }.drop(1);
        if (a.size() == 4)
        {
            if (is(a.front(), "if"))
            {
                return evaluate(args.at(0), stack).as_boolean() ? evaluate(args.at(1), stack) : evaluate(args.at(2), stack);
            }
        }
        else if (a.size() == 3)
        {
            if (is(a.front(), "let"))
            {
                return stack->insert(args.at(0).as_symbol(), evaluate(args.at(1), stack));
            }
            else if (is(a.front(), "lambda"))
            {
                return value::lambda_type{ args.at(0), args.at(1), *stack };
            }
        }
        else if (a.size() == 2)
        {
            if (is(a.front(), "quote") || is(a.front(), "'"))
            {
                return args.at(0);
            }
        }
        if (is(a.front(), "begin"))
        {
            value result = {};
            for (const value& arg : args)
            {
                result = evaluate(arg, stack);
            }
            return result;
        }
        if (is(a.front(), "cond"))
        {
            for (const auto& arg : args)
            {
                const auto& pair = arg.as_array();
                if (pair.size() != 2)
                {
                    throw std::runtime_error{ "cond: a list of pairs required" };
                }
                if (evaluate(pair[0], stack))
                {
                    return evaluate(pair[1], stack);
                }
            }
            throw std::runtime_error{ "cond: no match found" };
        }

        const std::vector<value> arg_values = std::invoke(
            [&]()
            {
                std::vector<value> result;
                result.reserve(args.size());
                std::transform(
                    std::begin(args),
                    std::end(args),
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
            auto new_frame = stack_type::frame_type{};
            for (std::size_t i = 0; i < params.size(); ++i)
            {
                new_frame.emplace(params.at(i).as_symbol(), arg_values.at(i));
            }

            auto new_stack = stack_type{ new_frame, stack };

            const auto fn = [&](const std::vector<value>& arguments) -> value { return evaluate(lambda.body, &new_stack); };
            return value::callable_type{ fn, "lambda", params.size() }(arg_values);
        }
        else
        {
            throw std::runtime_error{ str("Error on executing ", func) };
        }
    }
    else if (expr.is_symbol())
    {
        return (*stack)[expr.as_symbol()];
    }
    return expr;
}

}  // namespace lisp