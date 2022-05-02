#include <iostream>
#include <string>
#include <sstream>
#include <concepts>
#include <variant>
#include <type_traits>
#include <memory>
#include <any>

#include <Warp/AbstractSyntaxTreeUtilities.hpp>

using namespace Warp::Parser;

constexpr char char_cast( Warp::Parser::ExpressionOperator operation ) {
    return static_cast< char >( operation );
}

constexpr ctpg::nterm< Warp::AbstractSyntaxTree::VariantType > factor( "Factor" );
constexpr ctpg::nterm< Warp::AbstractSyntaxTree::VariantType > sum( "Sum" );
constexpr ctpg::nterm< Warp::AbstractSyntaxTree::VariantType > parenthesis_scope( "ParenthesisScope" );

constexpr char natural_number_regex[] = "[0-9][0-9]*";
constexpr ctpg::regex_term< natural_number_regex > natural_number_term( "NaturalNumber" );

constexpr ctpg::char_term plus_term( 
        char_cast( Warp::Parser::ExpressionOperator::SumAdd ), 1, ctpg::associativity::ltor 
    );
constexpr ctpg::char_term minus_term( 
        char_cast( Warp::Parser::ExpressionOperator::SumSubtract ), 1, ctpg::associativity::ltor 
    );
constexpr ctpg::char_term multiply_term( 
        char_cast( Warp::Parser::ExpressionOperator::FactorMultiply ), 2, ctpg::associativity::ltor 
    );
constexpr ctpg::char_term divide_term( 
        char_cast( Warp::Parser::ExpressionOperator::FactorDivide ), 2, ctpg::associativity::ltor 
    );
constexpr ctpg::char_term left_parenthesis_term( '(', 3, ctpg::associativity::ltor );
constexpr ctpg::char_term right_parenthesis_term( ')', 3, ctpg::associativity::ltor );


/* Before you ask... no, I could not use unique_ptr, in fact I could not manage ****
** the memory, for why you cant use unique_ptr see: ********************************
** See: https://github.com/cplusplus/papers/issues/961 *****************************
** and http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2273r0.pdf *********
** https://github.com/riscygeek/constexpr_suff/blob/master/include/unique_ptr.hpp */


constexpr ctpg::parser factor_parser( 
        factor, 
        ctpg::terms( 
                multiply_term, 
                divide_term, 
                plus_term, 
                minus_term, 
                natural_number_term, 
                left_parenthesis_term, 
                right_parenthesis_term 
            ), 
        ctpg::nterms( 
                factor, 
                sum, 
                parenthesis_scope 
            ), 
        ctpg::rules( 
                factor( natural_number_term ) 
                        >= []( auto token ) {
                                return Warp::Utilities::allocate_integral_literal_node< size_t >( token );
                            }, 
                factor( factor, multiply_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token )
                            {
                                return allocate_node< Warp::Parser::ExpressionOperator::FactorMultiply >( 
                                        current_factor, 
                                        Warp::Utilities::allocate_integral_literal_node< size_t >( next_token )
                                    );
                            }, 
                factor( factor, divide_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token )
                            {
                                return allocate_node< ExpressionOperator::FactorDivide >( 
                                        current_factor, 
                                        Warp::Utilities::allocate_integral_literal_node< size_t >( next_token )
                                    );
                            }, 
                parenthesis_scope( left_parenthesis_term, factor, right_parenthesis_term )
                        >= [] ( auto, auto factor, auto ) { return factor; }, 
                factor( parenthesis_scope ) >= []( auto parenthesis_scope ) { return parenthesis_scope; }, 
                factor( factor, multiply_term, parenthesis_scope ) 
                        >= []( auto factor, auto, auto parenthesis_scope ) 
                            {
                                return allocate_node< ExpressionOperator::FactorMultiply >( 
                                        factor, 
                                        parenthesis_scope 
                                    );
                            }, 
                factor( factor, divide_term, parenthesis_scope ) 
                        >= []( auto factor, auto, auto parenthesis_scope ) 
                            {
                                return allocate_node< ExpressionOperator::FactorDivide >( 
                                        factor, 
                                        parenthesis_scope 
                                    );
                            }, 
                factor( sum ) 
                        >= []( auto sum ) {
                            return sum;
                        }, 
                sum( factor, plus_term, factor ) 
                        >= []( auto current_sum, auto, const auto& next_token ) 
                            {
                                return allocate_node< ExpressionOperator::SumAdd >( 
                                        current_sum, 
                                        next_token 
                                    );
                            }, 
                sum( factor, minus_term, factor ) 
                        >= []( auto current_sum, auto, const auto& next_token )
                            {
                                return allocate_node< ExpressionOperator::SumSubtract >( 
                                        current_sum, 
                                        next_token 
                                    );
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
                            // ctpg::parse_options{}.set_verbose(), 
                            ctpg::buffers::string_buffer( input.c_str() ), std::cerr 
                    ); 
                    parse_result.has_value() == true 
                )
            parse_result.value();
            // std::cout << "Result: " << Warp::Utilities::tree_to_string( parse_result.value(), 0 ) << "\n";
        else
            std::cerr << "Error failed to parse!\n";
        std::cout << "Enter prompt please: ";
    }
    return 0;
}
