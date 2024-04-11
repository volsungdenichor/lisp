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
const auto sym_quote = symbol{ "quote" };
const auto sym_fwd_pipe = symbol{ "|>" };

std::vector<array> split_pipes(const array& a)
{
    if (a.empty())
    {
        return {};
    }
    const auto iter = std::find(std::begin(a), std::end(a), sym_fwd_pipe);
    if (iter != std::end(a))
    {
        return concat(
            vec(array(iterator_range{ std::begin(a), iter })),
            split_pipes(array(iterator_range{ std::next(iter), std::end(a) })));
    }
    else
    {
        return vec(a);
    }
}

array fold_pipes(const std::vector<array>& a)
{
    if (a.size() >= 1)
    {
        const auto last = a.back();
        const auto right = fold_pipes(iterator_range{ a } |= drop_back(1));
        if (right.empty())
        {
            return last;
        }
        else if (right.size() == 1 && right.at(0).is_symbol())
        {
            return concat(last, right);
        }
        else
        {
            return concat(last, array{ right });
        }
    }
    else if (a.size() == 1)
    {
        return a.front();
    }
    return {};
}

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
    const auto p = split_pipes(a);
    if (p.size() > 1)
    {
        return fold_pipes(p);
    }
    return {};
}

array apply_macro(const array& a)
{
    return do_apply_macro(a).value_or(a);
}

struct callable_lambda
{
    value::lambda_type lambda;
    value operator()(const std::vector<value>& args) const
    {
        const auto& params = lambda.params.as_array();
        auto new_frame = stack_type::frame_type{};
        for (std::size_t i = 0; i < params.size(); ++i)
        {
            new_frame.emplace(params.at(i).as_symbol(), args.at(i));
        }

        auto new_stack = stack_type{ new_frame, lambda.stack };

        return evaluate(lambda.body, &new_stack);
    }
};

struct evaluate_fn
{
    value operator()(const value& expr, stack_type* stack) const
    {
        if (expr.is_symbol())
        {
            return (*stack)[expr.as_symbol()];
        }
        else if (expr.is_array())
        {
            const auto a = apply_macro(expr.as_array());
            const auto args = iterator_range{ a } |= drop(1);
            if (a.size() == 4)
            {
                if (a[0] == sym_if)
                {
                    return (*this)(args.at(0), stack).as_boolean() ? (*this)(args.at(1), stack) : (*this)(args.at(2), stack);
                }
            }
            if (a.size() == 3)
            {
                if (a[0] == sym_let)
                {
                    return stack->insert(args.at(0).as_symbol(), (*this)(args.at(1), stack));
                }
                else if (a[0] == sym_lambda)
                {
                    const auto params = args.at(0);
                    const auto body = args.at(1);
                    const auto arity = params.as_array().size();
                    return value::callable_type{ callable_lambda{ value::lambda_type{ params, body, stack } },
                                                 str("lambda [", arity, "]"),
                                                 arity };
                }
            }
            if (a.size() == 2)
            {
                if (a[0] == sym_quote)
                {
                    return args.at(0);
                }
            }
            if (a[0] == sym_begin)
            {
                value result = {};
                for (const value& arg : args)
                {
                    result = (*this)(arg, stack);
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
                    if ((*this)(pair[0], stack))
                    {
                        return (*this)(pair[1], stack);
                    }
                }
                throw std::runtime_error{ "cond: no match found" };
            }

            const value op = (*this)(a[0], stack);

            const std::vector<value> arg_values = std::invoke(
                [&]()
                {
                    std::vector<value> result;
                    result.reserve(args.size());
                    std::transform(
                        std::begin(args),
                        std::end(args),
                        std::back_inserter(result),
                        [&](const value& v) { return (*this)(v, stack); });
                    return result;
                });

            try
            {
                return op.as_callable()(arg_values);
            }
            catch (const std::exception& ex)
            {
                std::stringstream ss;
                ss << "Exception: " << ex.what() << "\n"
                   << "Args:"
                   << "\n";
                for (std::size_t i = 0; i < arg_values.size(); ++i)
                {
                    ss << "[" << i << "] " << arg_values[i] << " <" << arg_values[i].get_category() << ">"
                       << "\n";
                }
                throw std::runtime_error{ ss.str() };
            }
        }
        return expr;
    }
};

value evaluate(const value& expr, stack_type* stack)
{
    return evaluate_fn{}(expr, stack);
}

}  // namespace lisp