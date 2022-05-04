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

// https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c
#include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#   include <cxxabi.h>
#endif
#include <memory>
#include <string>
#include <cstdlib>

template <class T>
std::string type_name()
{
    typedef typename std::remove_reference<T>::type TR;
    std::unique_ptr<char, void(*)(void*)> own
           (
#ifndef _MSC_VER
                abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                           nullptr, nullptr),
#else
                nullptr,
#endif
                std::free
           );
    std::string r = own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}


// using NodeInfoType = std::tuple< size_t, std::string >;

struct Meep {
    size_t a;
    std::string b;
};

using NodeInfoType = Meep;

template< size_t OffsetLengthParameterConstant, size_t LevelParameterConstant, size_t ParanetLeftParameterConstant, bool EnableParameterConstant >
NodeInfoType print_tree( const Warp::AbstractSyntaxTree::VariantType& variant );

// Warp::Parser::ExpressionOperator::FactorMultiply
// Warp::Parser::ExpressionOperator::FactorDivide
// Warp::Parser::ExpressionOperator::SumAdd
// Warp::Parser::ExpressionOperator::SumSubtract

std::string make_duplicate_string( const size_t amount_of_duplicates, char character_to_duplicate = 'd' )
{
    std::string buffer = "";
    for( size_t ii = 0; ii < amount_of_duplicates; ++ii )
        buffer += character_to_duplicate;
    return buffer;
}


size_t getlen( Meep m ) {
    return m.a;
}

std::string getstr( Meep m ) {
    return m.b;
}

// A really ugly functor. //
template< size_t OffsetLengthParameterConstant, size_t LevelParameterConstant, size_t ParanetLeftParameterConstant, bool EnableParameterConstant >
struct NodeToString
{
    NodeInfoType result;
    constexpr const static bool enabled = ( LevelParameterConstant < 80 );
    NodeToString( const auto& node )
    {
        const auto operation_heap_string = Warp::Utilities::to_string( std::remove_reference< decltype( node ) >::type::operation );
        auto operation = operation_heap_string.to_string_view();
        std::stringstream output;
        const auto left = print_tree< OffsetLengthParameterConstant * 2, LevelParameterConstant + 1, 0, enabled >( node.left );
        const auto right = print_tree< OffsetLengthParameterConstant * 2,  LevelParameterConstant + 1, 0, enabled >( node.right );
        const auto left_length = getlen( left );
        const auto right_length = getlen( right );
        const size_t next_length = left_length + right_length;

        const std::string this_offset = make_duplicate_string( OffsetLengthParameterConstant, ' ' );
        const std::string offset = make_duplicate_string( left_length, ' ' );
        const std::string half_offset = make_duplicate_string(  right_length, ' ' );
        output << "/" << operation << "\\"  << offset 
        << getstr( right ) << offset  << getstr( left ) ;
        // << ( ( is_left == true && parent_left == true ) ? "\n" : "" );
        result = NodeInfoType{ getlen( right ), output.str() };
    }

    NodeToString( const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Literal >& node  )
    {
        // std::cout << "L::"  << "parent_left: " << parent_left << " : is_left: " << is_left << "\n";
        std::stringstream output;
        const std::string offset = make_duplicate_string( OffsetLengthParameterConstant, ' ' );
        output << offset << *static_cast< size_t* >( node.value.factor.get_pointer()->get_data() ) ;
        result = NodeInfoType{ OffsetLengthParameterConstant, output.str() };
    }

};


template< size_t OffsetLengthParameterConstant, size_t LevelParameterConstant, size_t ParanetLeftParameterConstant >
struct NodeToString< 
        OffsetLengthParameterConstant, 
        LevelParameterConstant, 
        ParanetLeftParameterConstant, 
        false 
    >
{
    NodeInfoType result;
    NodeToString( const auto& node ) {
        result = NodeInfoType{ 0, "Recursion depth exceeded" };
    }

    NodeToString( const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Literal >& node  )
    {
        std::stringstream output;
        const std::string offset = make_duplicate_string( OffsetLengthParameterConstant, ' ' );
        output << offset << *static_cast< size_t* >( node.value.factor.get_pointer()->get_data() ) ;
        result = NodeInfoType{ OffsetLengthParameterConstant, output.str() };
    }

};

template< size_t OffsetLengthParameterConstant, size_t LevelParameterConstant, size_t ParanetLeftParameterConstant, bool EnableParameterConstant >
NodeInfoType print_tree( const Warp::AbstractSyntaxTree::VariantType& variant )
{
        const typename std::remove_pointer< decltype( variant.get_pointer() ) >::type& ptr = *variant.get_pointer();
        return Warp::Utilities::visit< []( auto x ) { 
                const std::string offset = make_duplicate_string( OffsetLengthParameterConstant, ' ' );
                return NodeToString< OffsetLengthParameterConstant, LevelParameterConstant, ParanetLeftParameterConstant, EnableParameterConstant >( *x ).result; 
            }>( ptr );    
}

int main( int argc, char** args )
{
        std::stringstream buffer;
    buffer << "test" << "\n";
    std::cout << buffer.str();
    buffer << "another_test";
    buffer << buffer.str() << "\n";

    std::cout << "Hello world! Please enter your expression: ";
    for( std::string input = "2"/*"4*10+2"*/; input != "thanks :)"; std::getline( std::cin, input ) )
    {
        std::cout << "Got: " << input << "\n";
        if( auto parse_result = factor_parser.parse( 
                            // ctpg::parse_options{}.set_verbose(), 
                            ctpg::buffers::string_buffer( input.c_str() ), std::cerr 
                    ); 
                    parse_result.has_value() == true 
                )
        {
            // parse_result.value();
            // std::cout << "Result: " << Warp::Utilities::tree_to_string( parse_result.value() ) << "\n";
            const auto& r= parse_result.value();
            std::cout << getstr( print_tree< 1, 0, 0, true >( r ) );
            std::cout << "\n";
        }
        else
            std::cerr << "Error failed to parse!\n";
        std::cout << "Enter prompt please: ";
    }
    return 0;
}
