#include <lisp/evaluate.hpp>
#include <lisp/utils/iterator_range.hpp>

namespace lisp
{

const auto sym_defun = symbol{ "defun" };
const auto sym_lambda = symbol{ "lambda" };
const auto sym_let = symbol{ "let" };
const auto sym_if = symbol{ "if" };
const auto sym_begin = symbol{ "begin" };
const auto sym_cond = symbol{ "cond" };

std::optional<array> do_apply_macro(const array& a)
{
    if (a.size() == 4 && a.at(0) == sym_defun)
    {
        return array{ sym_let, a.at(1), array{ sym_lambda, a.at(2), a.at(3) } };
    }
    if (a.size() == 3 && a.at(1) == symbol{ ":=" })
    {
        return array{ sym_let, a.at(0), a.at(2) };
    }
    return {};
}

array apply_macro(const array& a)
{
    return do_apply_macro(a).value_or(a);
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
            if (a[0] == sym_if)
            {
                return evaluate(args.at(0), stack).as_boolean() ? evaluate(args.at(1), stack) : evaluate(args.at(2), stack);
            }
        }
        else if (a.size() == 3)
        {
            if (a[0] == sym_let)
            {
                return stack->insert(args.at(0).as_symbol(), evaluate(args.at(1), stack));
            }
            else if (a[0] == sym_lambda)
            {
                return value::lambda_type{ args.at(0), args.at(1), *stack };
            }
        }
        else if (a.size() == 2)
        {
            if (is(a[0], "quote") || is(a[0], "'"))
            {
                return args.at(0);
            }
        }
        if (a[0] == sym_begin)
        {
            value result = {};
            for (const value& arg : args)
            {
                result = evaluate(arg, stack);
            }
            return result;
        }
        if (a[0] == sym_cond)
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
            try
            {
                return func.as_callable()(arg_values);
            }
            catch (const std::exception& ex)
            {
                throw std::runtime_error{ str("on evaluation of ", func, ": ", ex.what()) };
            }
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
            return value::callable_type{ fn, "lambda" }(arg_values);
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