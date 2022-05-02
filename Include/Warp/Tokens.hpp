namespace Warp::Parser
{
    constexpr static char star_token = '*';
    constexpr static char forward_slash_token = '/';
    constexpr static char plus_token = '+';
    constexpr static char minus_token = '-';
    constexpr static char caret_token = '^';
    constexpr static char colon_token = ':';

    constexpr static char open_parenthesis = '(';
    constexpr static char close_parenthesis = ')';
    constexpr static char open_curly_bracket = '{';
    constexpr static char close_curly_bracket = '}';
    constexpr static char open_square_bracket = '[';
    constexpr static char close_square_bracket = ']';
    constexpr static char open_angle_bracket = '<';
    constexpr static char close_angle_bracket = '>';

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
}
