#include <iostream>
#include <functional>
#include <ctpg.hpp>
#include <Warp/SanityCheck.hpp>

// template< typename OperhandParameterType >
// using BinaryOperationType = std::function< OperhandParameterType( OperhandParameterType, OperhandParameterType ) >;

constexpr char natural_number_regex[] = "[0-9][0-9]*";
constexpr ctpg::regex_term< natural_number_regex > natural_number_term( "NaturalNumber" );

// No unicode support :(
constexpr char identifier_regex[] = "[a-zA-Z\\_][a-zA-Z0-9\\_]*";
constexpr ctpg::regex_term< identifier_regex > identifier( "Identifier" );

constexpr ctpg::nterm< size_t > factor( "Factor" );
constexpr ctpg::nterm< size_t > sum( "Sum" );
constexpr ctpg::nterm< size_t > parenthesis_scope( "ParenthesisScope" );

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
constexpr ctpg::char_term left_parenthesis_term( '(', 3, ctpg::associativity::ltor );
constexpr ctpg::char_term right_parenthesis_term( ')', 3, ctpg::associativity::ltor );
// constexpr ctpg::char_ter m geq( ">=", 1, ctpg::associativity::ltor );


constexpr ctpg::string_term plus_eq_term( "+=", 1, ctpg::associativity::ltor );

// TermBuffer< '+', '-' 

constexpr ctpg::string_term plus_equals_term( "+=", 1, ctpg::associativity::ltor );

constexpr ctpg::parser factor_parser( 
        factor, 
        ctpg::terms( 
                multiply_term, 
                divide_term, 
                plus_term, 
                minus_term, 
                natural_number_term, 
                left_parenthesis_term, 
                right_parenthesis_term, 
                identifier, 
                plus_eq_term
            ), 
        ctpg::nterms( factor, sum, parenthesis_scope ), 
        ctpg::rules( 
                factor( natural_number_term ) >= to_size_t, 
                factor( factor, multiply_term, natural_number_term ) 
                        >= []( size_t current_factor, auto, const auto& next_token ) {  
                                return current_factor * to_size_t( next_token ); 
                            }, 
                factor( factor, divide_term, natural_number_term ) 
                        >= []( size_t current_factor, auto, const auto& next_token ) {
                                return current_factor / to_size_t( next_token ); 
                            },
                parenthesis_scope( left_parenthesis_term, factor, right_parenthesis_term )
                        >= [] ( auto, auto factor, auto ) { return factor; }, 
                factor( parenthesis_scope ) >= []( auto parenthesis_scope ) { return parenthesis_scope; }, 
                factor( factor, multiply_term, parenthesis_scope ) 
                        >= []( auto factor, auto, auto parenthesis_scope ) { 
                                return factor * parenthesis_scope; 
                            }, 
                factor( factor, divide_term, parenthesis_scope ) 
                        >= []( auto factor, auto, auto parenthesis_scope ) { 
                                return factor / parenthesis_scope; 
                            }, 
                factor( sum ) >= []( auto sum ) { return sum; }, 
                sum( factor, plus_term, factor ) 
                        >= []( auto current_sum, auto, const auto& next_token ) {
                                return  current_sum + next_token; 
                            }, 
                sum( factor, plus_eq_term, factor ) 
                        >= []( auto current_sum, auto, const auto& next_token ) {
                            std::cout << "PLUS EQ\n";
                                return  current_sum + next_token; 
                            }, 
                sum( factor, minus_term, factor ) 
                        >= []( auto current_sum, auto, const auto& next_token ) {
                                return current_sum - next_token; 
                            } 
           )
);

int main( int argc, char** args )
{
    std::cout << "Hello world! Please enter your expression: ";
    for( std::string input = "4*10+2"; input != "thanks :)"; std::getline( std::cin, input ) )
    {
        std::cout << "Got: " << input << "\n";
        if( auto parse_result = factor_parser.parse( 
                            //ctpg::parse_options{}.set_verbose(), 
                            ctpg::buffers::string_buffer( input.c_str() ), std::cerr 
                    ); 
                    parse_result.has_value() == true 
                )
            std::cout << "Result: " << parse_result.value() << "\n";
        else
            std::cerr << "Error failed to parse!\n";
        std::cout << "Enter prompt please: ";
    }
    return 0;
}
