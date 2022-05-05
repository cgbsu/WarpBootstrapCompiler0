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
    constexpr static const char not_operator = '!';
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_TOKENS_HPP
