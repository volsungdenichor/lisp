#include <gmock/gmock.h>

#include <lisp/default_stack.hpp>
#include <lisp/evaluate.hpp>
#include <lisp/parser.hpp>

lisp::value eval(std::string_view code)
{
    lisp::stack_type stack = lisp::default_stack();
    const auto val = lisp::parse(code);
    return lisp::evaluate(val, &stack);
}

testing::Matcher<const lisp::value&> Approx(double v)
{
    return testing::ResultOf(std::mem_fn(&lisp::value::as_floating_point), testing::DoubleEq(v));
}

TEST(expr, atoms)
{
    EXPECT_THAT(eval("5"), 5);
    EXPECT_THAT(eval("5.14"), Approx(5.14));
    EXPECT_THAT(eval("true"), true);
    EXPECT_THAT(eval("false"), false);
}

TEST(expr, quote)
{
    EXPECT_THAT(eval("(quote (2 3 4))"), (lisp::array{ 2, 3, 4 }));
    EXPECT_THAT(eval("'(2 (3 14) 4)"), (lisp::array{ 2, lisp::array{ 3, 14 }, 4 }));
}

TEST(expr, arithmetic_operators)
{
    EXPECT_THAT(eval("(+ 2 3)"), 5);
    EXPECT_THAT(eval("(- 2 3)"), -1);
    EXPECT_THAT(eval("(* 2 3)"), 6);
    EXPECT_THAT(eval("(/ 2 3)"), 0);
}

TEST(expr, comparison)
{
    EXPECT_THAT(eval("(== 3 3)"), true);
    EXPECT_THAT(eval("(== 3 5)"), false);
    EXPECT_THAT(eval("(== 5 3)"), false);

    EXPECT_THAT(eval("(!= 3 3)"), false);
    EXPECT_THAT(eval("(!= 3 5)"), true);
    EXPECT_THAT(eval("(!= 5 3)"), true);

    EXPECT_THAT(eval("(< 3 3)"), false);
    EXPECT_THAT(eval("(< 3 5)"), true);
    EXPECT_THAT(eval("(< 5 3)"), false);

    EXPECT_THAT(eval("(< 3 3)"), false);
    EXPECT_THAT(eval("(< 3 5)"), true);
    EXPECT_THAT(eval("(< 5 3)"), false);

    EXPECT_THAT(eval("(> 3 3)"), false);
    EXPECT_THAT(eval("(> 3 5)"), false);
    EXPECT_THAT(eval("(> 5 3)"), true);

    EXPECT_THAT(eval("(<= 3 3)"), true);
    EXPECT_THAT(eval("(<= 3 5)"), true);
    EXPECT_THAT(eval("(<= 5 3)"), false);

    EXPECT_THAT(eval("(>= 3 3)"), true);
    EXPECT_THAT(eval("(>= 3 5)"), false);
    EXPECT_THAT(eval("(>= 5 3)"), true);
}
