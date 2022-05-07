#include <Warp/Common.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_TOKENS_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_TOKENS_HPP

namespace Warp::Parser
{
    constexpr static const char star_token = '*';
    constexpr static const char forward_slash_token = '/';
    constexpr static const char plus_token = '+';
    constexpr static const char minus_token = '-';
    constexpr static const char caret_token = '^';
    constexpr static const char colon_token = ':';
    constexpr static const char semi_colon_token = ';';
    constexpr static const char exclimation_point = '!';

    constexpr static const char open_parenthesis = '(';
    constexpr static const char close_parenthesis = ')';
    constexpr static const char open_curly_bracket = '{';
    constexpr static const char close_curly_bracket = '}';
    constexpr static const char open_square_bracket = '[';
    constexpr static const char close_square_bracket = ']';
    constexpr static const char open_angle_bracket = '<';
    constexpr static const char close_angle_bracket = '>';

    enum class ExpressionOperator : char 
    {
        FactorMultiply = star_token, 
        FactorDivide = forward_slash_token, 
        SumAdd = plus_token, 
        SumSubtract = minus_token, 
        ExponentRaise = caret_token 
    };

    enum class BooleanOperator : char 
    {
        LogicalNot = exclimation_point, 
        LogicalAnd = '&', // = 0, 
        LogicalOr = '|', // = 1, 
        LogicalBiConditional = '@', // = 2, 
        LogicalImplies = '#'//= 3
    };

    enum class ScopeOperators : char 
    {
        OpenParenthesis = open_parenthesis, 
        CloseParenthesis = close_parenthesis, 
        OpenCurlyBracket = open_curly_bracket, 
        CloseCurlyBracket = close_curly_bracket, 
        OpenSquareBracket = open_square_bracket, 
        CloseSquareBracket = close_square_bracket, 
        OpenAngleBracket = open_angle_bracket, 
        CloseAngleBracket = close_angle_bracket 
    };

    constexpr static const char and_operator[] = "&&";
    constexpr static const char or_operator[] = "||";
    constexpr static const char bi_conditional_operator[] = "<->";
    constexpr static const char implies_operator[] = "->";

    // Unfortunatley, string concatination will just be too much of //
    // a problem with constexpr constraint. //////////////////////////
    #define MAKE_BOOL_VALUES( TRUE_TOKEN, FALSE_TOKEN ) \
        constexpr static const char true_token[] = #TRUE_TOKEN ; \
        constexpr static const char false_token[] = #FALSE_TOKEN ; \
        constexpr static const char bool_literal_regex[] =  "(" #TRUE_TOKEN ")\\b|(" #FALSE_TOKEN ")\\b"; // This is valid C++ code, string literals next to each-otehr become one literal/string //

    MAKE_BOOL_VALUES( true, false )

    // constexpr static const char definition_operator[] = "::";

    // constexpr static const char let_keyword[] = "let";


}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_TOKENS_HPP
