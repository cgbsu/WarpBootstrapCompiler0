#include <iostream>
#include <ctpg.hpp>

constexpr char natural_number_regex[] = "[0-9][0-9]*";
constexpr ctpg::regex_term< natural_number_regex > natural_number_term( "NaturalNumber" );

constexpr char factor_regex[] = "[*/]";
constexpr ctpg::regex_term< factor_regex > factor_operator_term( "FactorOperator" );

constexpr ctpg::nterm< size_t > factor( "Factor" );


// This function was "yoiked" directly from https://github.com/peter-winter/ctpg //
constexpr size_t to_size_t( std::string_view integer_token )
{
    size_t sum = 0;
    for( auto digit : integer_token )
        sum = ( sum * 10 ) + digit - '0';
    return sum;
}

constexpr ctpg::parser factor_parser( 
        factor, 
        ctpg::terms( '*', '/', natural_number_term ), 
        ctpg::nterms( factor ), 
        ctpg::rules(
                factor( natural_number_term ) >= to_size_t, 
                factor( factor, '*', natural_number_term ) 
                        >= []( size_t sum, auto factor_operator, const auto& next_token ) { // This lambda also yoikned. //
                                return sum * to_size_t( next_token ); 
                            }, 
                factor( factor, '/', natural_number_term ) 
                        >= []( size_t sum, auto factor_operator, const auto& next_token ) { // This lambda also yoikned. //
                                return sum / to_size_t( next_token ); 
                            }
            )
);

int main( int argc, char** args )
{
    auto parse_result = factor_parser.parse( 
            ctpg::buffers::string_buffer( args[ 1 ] ), std::cerr 
        );
    if( parse_result.has_value() == true )
        std::cout << parse_result.value() << "\n";
    else
        std::cerr << "Error failed to parse!\n";
    return 0;
}
