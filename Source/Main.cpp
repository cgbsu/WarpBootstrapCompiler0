#include <iostream>
#include <string>
#include <sstream>
#include <concepts>
#include <variant>
#include <type_traits>
#include <utility>
#include <memory>
#include <any>
#include <ctpg.hpp>


constexpr char star_token = '*';
constexpr char forward_slash_token = '/';
constexpr char plus_token = '+';
constexpr char minus_token = '-';
constexpr char caret_token = '^';
constexpr char colon_token = ':';

constexpr char open_parenthesis = '(';
constexpr char close_parenthesis = ')';
constexpr char open_curly_bracket = '{';
constexpr char close_curly_bracket = '}';
constexpr char open_square_bracket = '[';
constexpr char close_square_bracket = ']';
constexpr char open_angle_bracket = '<';
constexpr char close_angle_bracket = '>';

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

enum class NodeType
{
    Factor, 
    Sum, 
    Literal 
};

namespace Utility
{

    template< std::integral ParameterType >
    constexpr ParameterType to_integral( std::string_view integer_token )
    {
        ParameterType sum = 0;
        for( auto digit : integer_token )
            sum = ( sum * 10 ) + digit - '0';
        return sum;
    }

    template< typename ParameterType >
    constexpr std::string_view to_string( ParameterType to_stringify ) {
        return std::string_view{ to_stringify };
    }

    template<>
    constexpr std::string_view to_string( char to_stringify ) {
        const char string[] = { to_stringify, '\0' };
        return std::string_view{ string };
    }

    template< typename ParameterType >
    concept Enumaration = std::is_enum< ParameterType >::value;

    template< Enumaration EnumerationParameterType >
    constexpr std::string_view to_string( EnumerationParameterType to_stringify )
    {
        using UnderylingType = std::underlying_type_t< decltype( to_stringify ) >;
        return to_string( static_cast< UnderylingType >( to_stringify ) );
    }

    template< std::integral IntegralParameterType >
    constexpr std::string_view to_string( IntegralParameterType to_stringify ) {
        return std::string_view{ std::to_string( to_stringify ) };
    }

    template< typename... AlternativeParameterTypes >
    constexpr std::string_view to_string( std::variant< AlternativeParameterTypes... > to_stringify ) {
        return std::visit( []( auto data ) { return to_string< decltype( data ) >( data ); }, to_stringify );
    }

}

struct ConstexprStringable
{
    constexpr virtual std::string_view to_string() const = 0;
    constexpr operator std::string_view() const {
        return to_string();
    }
};

template< typename... ArithmaticParameterTypes >
struct StrongFactor : public ConstexprStringable
{
    using FactorType = std::variant< ArithmaticParameterTypes... >;
    using ThisType = StrongFactor< ArithmaticParameterTypes... >;
    using ComparisionResultType = std::common_comparison_category_t< std::compare_three_way_result_t< ArithmaticParameterTypes >... >;

    const FactorType factor;
    
    constexpr StrongFactor( FactorType factor ) : factor( factor ) {}
    constexpr StrongFactor( auto factor ) : factor( factor ) {}

    constexpr virtual std::string_view to_string() const override final {
        return Utility::to_string( factor );
    }

    constexpr ThisType operator*( const auto other ) {
        return ThisType{ *std::get_if< decltype( other ) >( &factor ) * other };
    }
    constexpr ThisType operator/( const auto other ) {
        return ThisType{ *std::get_if< decltype( other ) >( &factor ) / other };
    }
    constexpr ThisType operator+( const auto other ) {
        return ThisType{ *std::get_if< decltype( other ) >( &factor ) + other };
    }
    constexpr ThisType operator-( const auto other ) {
        return ThisType{ *std::get_if< decltype( other ) >( &factor ) - other };
    }
    constexpr ComparisionResultType operator<=>( const auto other ) {
        return factor.operator<=>( other );
    }
};

using LiteralType = StrongFactor< size_t, signed long long int >;

template< auto >
struct Node {};

// I dont remember this being such a problem https://stackoverflow.com/questions/652155/invalid-use-of-incomplete-type

struct Node< NodeType::Factor >;
struct Node< NodeType::Sum >;
struct Node< NodeType::Literal >;

using VariantType = std::variant< 
    const Node< NodeType::Factor >*, 
    const Node< NodeType::Sum >*, 
    const Node< NodeType::Literal >* 
>;

struct LeftRight : public ConstexprStringable
{
    const VariantType left;
    const VariantType right;
    constexpr LeftRight( const VariantType left, const VariantType right ) 
            : left( left ), right( right ) {}
};

template<>
struct Node< NodeType::Literal > : public ConstexprStringable
{
    const LiteralType value;
    constexpr Node( LiteralType value ) : value( value ) {}
    constexpr Node( auto value ) : value( value ) {}
    constexpr Node( Node< NodeType::Literal > const& other ) : value( other.value ) {}
    constexpr virtual std::string_view to_string() const override final {
        return value.to_string();
    }
};

template<>
struct Node< NodeType::Factor > : public LeftRight
{
    using BaseType = LeftRight;
    const ExpressionOperator operation;
    constexpr Node( 
            const VariantType left, 
            const ExpressionOperator operation, 
            const VariantType right 
        ) : BaseType( left, right ), operation( operation ) {}
    constexpr Node( Node< NodeType::Factor > const& other ) 
            : BaseType( other.left, other.right ), 
                    operation( other.operation ) {}
        constexpr static const char* debug = "Factor";
    constexpr virtual std::string_view to_string() const override final {
        return debug;
    }
};

template<>
struct Node< NodeType::Sum > : public LeftRight
{
    using BaseType = LeftRight;
    const ExpressionOperator operation;
    constexpr Node( 
            const VariantType left, 
            const ExpressionOperator operation, 
            const VariantType right 
        ) : BaseType( left, right ), operation( operation ) {}
    constexpr Node( Node< NodeType::Sum > const& other ) 
            : BaseType( other.left, other.right ), 
                    operation( other.operation ) {}
        constexpr static const char* debug = "Sum";
    constexpr virtual std::string_view to_string() const override final {
        return debug;
    }

};

constexpr char char_cast( ExpressionOperator operation ) {
    return static_cast< char >( operation );
}

constexpr ctpg::nterm< VariantType > factor( "Factor" );
constexpr ctpg::nterm< VariantType > sum( "Sum" );
constexpr ctpg::nterm< VariantType > parenthesis_scope( "ParenthesisScope" );

constexpr char natural_number_regex[] = "[0-9][0-9]*";
constexpr ctpg::regex_term< natural_number_regex > natural_number_term( "NaturalNumber" );

constexpr ctpg::char_term plus_term( 
        char_cast( ExpressionOperator::SumAdd ), 1, ctpg::associativity::ltor 
    );
constexpr ctpg::char_term minus_term( 
        char_cast( ExpressionOperator::SumSubtract ), 1, ctpg::associativity::ltor 
    );
constexpr ctpg::char_term multiply_term( 
        char_cast( ExpressionOperator::FactorMultiply ), 2, ctpg::associativity::ltor 
    );
constexpr ctpg::char_term divide_term( 
        char_cast( ExpressionOperator::FactorDivide ), 2, ctpg::associativity::ltor 
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
                                return VariantType{ new Node< NodeType::Literal >{ 
                                        Utility::to_integral< size_t >( token ) 
                                    } };
                            }, 
                factor( factor, multiply_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token )
                            {
                                using Type = Node< NodeType::Factor >;
                                return VariantType{ new Type { 
                                        current_factor, 
                                        ExpressionOperator::FactorMultiply, 
                                        VariantType{ new Node< NodeType::Literal >{ 
                                                Utility::to_integral< size_t >( next_token ) 
                                            } } 
                                    } };
                            }, 
                factor( factor, divide_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token )
                            {
                                using Type = Node< NodeType::Factor >;
                                return VariantType{ new Type { 
                                        current_factor, 
                                        ExpressionOperator::FactorDivide, 
                                        VariantType{ new Node< NodeType::Literal >{ 
                                                Utility::to_integral< size_t >( next_token ) 
                                            } } 
                                    } };
                            }, 
                parenthesis_scope( left_parenthesis_term, factor, right_parenthesis_term )
                        >= [] ( auto, auto factor, auto ) { return factor; }, 
                factor( parenthesis_scope ) >= []( auto parenthesis_scope ) { return parenthesis_scope; }, 
                factor( factor, multiply_term, parenthesis_scope ) 
                        >= []( auto factor, auto, auto parenthesis_scope ) 
                            {
                                using Type = Node< NodeType::Factor >;
                                return VariantType{ new Type { 
                                        factor, 
                                        ExpressionOperator::FactorMultiply, 
                                        parenthesis_scope 
                                    } };
                            }, 
                factor( factor, divide_term, parenthesis_scope ) 
                        >= []( auto factor, auto, auto parenthesis_scope ) 
                            {
                                using Type = Node< NodeType::Factor >;
                                return VariantType{ new Type { 
                                        factor, 
                                        ExpressionOperator::FactorDivide, 
                                        parenthesis_scope 
                                    } };
                            }, 
                factor( sum ) 
                        >= []( auto sum ) {
                            return VariantType{ sum };
                        }, 
                sum( factor, plus_term, factor ) 
                        >= []( auto current_sum, auto, const auto& next_token ) 
                            {
                                using Type = Node< NodeType::Sum >;
                                return VariantType{ new Type { 
                                        current_sum, 
                                        ExpressionOperator::SumAdd, 
                                        next_token 
                                    } };
                            }, 
                sum( factor, minus_term, factor ) 
                        >= []( auto current_sum, auto, const auto& next_token )
                            {
                                using Type = Node< NodeType::Sum >;
                                return VariantType{ new Type { 
                                        current_sum, 
                                        ExpressionOperator::SumSubtract, 
                                        next_token 
                                    } };
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
        {
            // char* result;
            std::cout << "Result: " << std::visit( [ & ]( auto data ) { return data->to_string(); }, parse_result.value() ) << "\n";
        }
        else
            std::cerr << "Error failed to parse!\n";
        std::cout << "Enter prompt please: ";
    }
    return 0;
}
