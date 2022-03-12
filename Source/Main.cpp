#include <iostream>
#include <ctpg.hpp>
#include <Warp/SanityCheck.hpp>

constexpr char star_token = '*';
constexpr char forward_slash_token = '/';
constexpr char plus_token = '+';
constexpr char minus_token = '+';
constexpr char caret_token = '^';

enum class ExpressionOperators : char 
{
    FactorMultiply = star_token, 
    FactorDivide = forward_slash_token, 
    SumAdd = plus_token, 
    SumSubtract = minus_token, 
    ExponentRaise = caret_token 
    // ExponentSquareRoot = ;
    // Factorial = 
};

constexpr char natural_number_regex[] = "[0-9][0-9]*";
constexpr ctpg::regex_term< natural_number_regex > natural_number_term( "NaturalNumber" );

constexpr ctpg::nterm< size_t > factor( "Factor" );
constexpr ctpg::nterm< size_t > sum( "Sum" );


// This function was "yoiked" directly from https://github.com/peter-winter/ctpg //
constexpr size_t to_size_t( std::string_view integer_token )
{
    size_t sum = 0;
    for( auto digit : integer_token )
        sum = ( sum * 10 ) + digit - '0';
    return sum;
}

constexpr ctpg::char_term plus_term( '+', 1, ctpg::associativity::ltor );
constexpr ctpg::char_term minus_term( '-', 1, ctpg::associativity::ltor );
constexpr ctpg::char_term multiply_term( '*', 2, ctpg::associativity::ltor );
constexpr ctpg::char_term divide_term( '/', 2, ctpg::associativity::ltor );



constexpr ctpg::parser factor_parser( 
        factor, 
        ctpg::terms( multiply_term, divide_term, plus_term, minus_term, natural_number_term ), 
        ctpg::nterms( factor, sum ), 
        ctpg::rules(
                factor( natural_number_term ) >= to_size_t, 
                // sum( natural_number_term ) >= to_size_t, 
                factor( factor, multiply_term, natural_number_term ) 
                        >= []( size_t current_factor, auto factor_operator, const auto& next_token ) { // This lambda also yoikned. //
                                return current_factor * to_size_t( next_token ); 
                            }, 
                factor( factor, divide_term, natural_number_term ) 
                        >= []( size_t current_factor, auto factor_operator, const auto& next_token ) { // This lambda also yoikned. //
                                return current_factor / to_size_t( next_token ); 
                            },
                factor( sum ) >= []( auto sum ) { return sum; }, 
                sum( factor, plus_term, factor ) 
                        >= []( auto current_sum, auto sum_operator, const auto& next_token ) {
                                return  current_sum + next_token; 
                            }, 
                sum( factor, minus_term, factor ) 
                        >= []( auto current_sum, auto sum_operator, const auto& next_token ) {
                                return current_sum - next_token; 
                            } 
           )
);

int main( int argc, char** args )
{

    auto parse_result = factor_parser.parse( 
            ctpg::parse_options{}.set_verbose(), 
            ctpg::buffers::string_buffer( args[ 1 ] ), std::cerr 
        );
    if( parse_result.has_value() == true )
        std::cout << parse_result.value() << "\n";
    else
        std::cerr << "Error failed to parse!\n";
    return 0;
}