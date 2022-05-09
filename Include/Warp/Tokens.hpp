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
    constexpr static const char exclimation_point_token = '!';
    constexpr static const char equal_sign_token = '=';
    constexpr static const char comma_token = ',';

    constexpr static const char open_parenthesis_token = '(';
    constexpr static const char close_parenthesis_token = ')';
    constexpr static const char open_curly_bracket_token = '{';
    constexpr static const char close_curly_bracket_token = '}';
    constexpr static const char open_square_bracket_token = '[';
    constexpr static const char close_square_bracket_token = ']';
    constexpr static const char open_angle_bracket_token = '<';
    constexpr static const char close_angle_bracket_token = '>';

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
        LogicalNot = exclimation_point_token, 
        LogicalAnd = '1', 
        LogicalOr = '2', 
        LogicalBiCondition = '3', 
        LogicalImplies = '4' 
    };

    enum class ComparisonOperator : char 
    {
        ComparisonEqual = equal_sign_token, 
        ComparisonLessThan = open_angle_bracket_token, 
        ComparisonGreaterThan = close_angle_bracket_token, 
        ComparisionLessThanOrEqualTo = '5', 
        ComparisionGreaterThanOrEqualTo = '6', 
    };

    enum class ScopeOperators : char 
    {
        OpenParenthesis = open_parenthesis_token, 
        CloseParenthesis = close_parenthesis_token, 
        OpenCurlyBracket = open_curly_bracket_token, 
        CloseCurlyBracket = close_curly_bracket_token, 
        OpenSquareBracket = open_square_bracket_token, 
        CloseSquareBracket = close_square_bracket_token, 
        OpenAngleBracket = open_angle_bracket_token, 
        CloseAngleBracket = close_angle_bracket_token 
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

    enum class FunctionOperators
    {
        FunctionParameterNextParameter = comma_token, 
        FunctionDefintionComplete = semi_colon_token, 
        FunctionParameterConstaraint = colon_token, 
    };
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_TOKENS_HPP
