#include <iostream>
#include <concepts>
#include <variant>
#include <type_traits>

#include <ctpg.hpp>

constexpr char star_token = '*';
constexpr char forward_slash_token = '/';
constexpr char plus_token = '+';
constexpr char minus_token = '+';
constexpr char caret_token = '^';
constexpr char colon_token = ':';

enum class ExpressionOperators : char 
{
    FactorMultiply = star_token, 
    FactorDivide = forward_slash_token, 
    SumAdd = plus_token, 
    SumSubtract = minus_token, 
    ExponentRaise = caret_token 
};

enum class BooleanOperators : size_t
{
    LessThan = 0, 
    GreaterThan = 1, 
    EqualTo = 2, 
    LessThanOrEqualTo = 3, 
    GreaterThanOrEqualTo = 4, 
    And = 5, 
    Or = 6 
};

// template< typename CanidateType >
// concept ArithmaticConcept = requires( const CanidateType canidate )
// {
//     { canidate * canidate } -> std::convertible_to< CanidateType >;
//     { canidate / canidate } -> std::convertible_to< CanidateType >;
//     { canidate - canidate } -> std::convertible_to< CanidateType >;
//     { canidate + canidate } -> std::convertible_to< CanidateType >;
// };

template< typename... ArithmaticParameterTypes >
struct LiteralFactor
{
    using FactorType = std::variant< ArithmaticParameterTypes... >;
    using ThisType = LiteralFactor< ArithmaticParameterTypes... >;
    const FactorType factor;
    
    explicit constexpr LiteralFactor( FactorType factor ) : factor( factor ) {}

    constexpr ThisType operator*( const auto other ) {
        return ThisType{ *std::get_if< decltype( other ) >( &factor ) * other };
    }
    constexpr ThisType operator/( const auto other ) {
        return ThisType{ *std::get_if< decltype( other ) >( &factor ) * other };
    }
    constexpr ThisType operator_( const auto other ) {
        return ThisType{ *std::get_if< decltype( other ) >( &factor ) * other };
    }
    constexpr ThisType operator-( const auto other ) {
        return ThisType{ *std::get_if< decltype( other ) >( &factor ) * other };
    }
    constexpr bool operator<=>( const auto other ) {
        return factor.operator<=>( other );
    }
};

using FactorType = LiteralFactor< size_t, int, float, double >;

template< typename, auto, typename... >
struct ExpressionNode
{
    static_assert( "Error::Attempt to instantiate unconstrained expression" );
};

template<>
struct ExpressionNode< FactorType, nullptr > : public FactorType
{

};

template< 
        ArithmaticConcept LeftParameterType, 
        ExpressionOperators OperationParameterConstant,  
        ArithmaticConcept RightParameterType 
    >
struct ExpressionNode< LeftParameterType, OperationParameterConstant, RightParameterType >
{
    constexpr static const ExpressionOperators operation = OperationParameterConstant;
    using LeftType = LeftParameterType;
    using RightType = RightParameterType;
    const LeftType left;
    const RightType right;
    using ThisType = ExpressionNode< LeftType, OperationParameterConstant, RightType >;

    ExpressionNode( const LeftType left, const RightType right ) 
            : left( left ), right( right ) {}

    constexpr auto operator*( auto other )
    {
        return ExpressionNode< 
                ThisType, 
                ExpressionOperators::FactorMultiply, 
                decltype( other ) 
            >{ ThisType{ left, right }, other };
    }
    constexpr auto operator/( const auto& other )
    {
        return ExpressionNode< 
                ThisType, 
                ExpressionOperators::FactorDivide, 
                decltype( other ) 
            >{ ThisType{ left, right }, other };
    }
    constexpr auto operator+( const auto& other )
    {
        return ExpressionNode< 
                ThisType, 
                ExpressionOperators::SumAdd, 
                decltype( other ) 
            >{ ThisType{ left, right }, other };
    }
    constexpr auto operator*( const auto& other )
    {
        return ExpressionNode< 
                ThisType, 
                ExpressionOperators::SumSubtract, 
                decltype( other ) 
            >{ ThisType{ left, right }, other };
    }
};

template< 
        ArithmaticConcept LeftParameterType, 
        BooleanOperators OperationParameterConstant,  
        ArithmaticConcept RightParameterType 
    >
struct ExpressionNode< LeftParameterType, OperationParameterConstant, RightParameterType >
{
    constexpr static const ExpressionOperators operation = OperationParameterConstant;
    using LeftType = LeftParameterType;
    using RightType = RightParameterType;
    const LeftType left;
    const RightType right;
    using ThisType = ExpressionNode< LeftType, OperationParameterConstant, RightType >;

    ExpressionNode( const LeftType left, const RightType right ) 
            : left( left ), right( right ) {}

    constexpr auto operator==( auto other )
    {
        return ExpressionNode< 
                ThisType, 
                BooleanOperators::EqualTo, 
                decltype( other ) 
            >{ ThisType{ left, right }, other };
    }
    constexpr auto operator<( const auto& other )
    {
        return ExpressionNode< 
                ThisType, 
                BooleanOperators::LessThan, 
                decltype( other ) 
            >{ ThisType{ left, right }, other };
    }
    constexpr auto operator>( const auto& other )
    {
        return ExpressionNode< 
                ThisType, 
                BooleanOperators::GreaterThan, 
                decltype( other ) 
            >{ ThisType{ left, right }, other };
    }
    constexpr auto operator>=( const auto& other )
    {
        return ExpressionNode< 
                ThisType, 
                BooleanOperators::GreaterThanOrEqualTo, 
                decltype( other ) 
            >{ ThisType{ left, right }, other };
    }
    constexpr auto operator<=( const auto& other )
    {
        return ExpressionNode< 
                ThisType, 
                BooleanOperators::LessThanOrEqualTo, 
                decltype( other ) 
            >{ ThisType{ left, right }, other };
    }
    constexpr auto operator&&( const auto& other )
    {
        return ExpressionNode< 
                ThisType, 
                BooleanOperators::And, 
                decltype( other ) 
            >{ ThisType{ left, right }, other };
    }
    constexpr auto operator||( const auto& other )
    {
        return ExpressionNode< 
                ThisType, 
                BooleanOperators::Or, 
                decltype( other ) 
            >{ ThisType{ left, right }, other };
    }
};

constexpr char natural_number_regex[] = "[0-9][0-9]*";
constexpr ctpg::regex_term< natural_number_regex > natural_number_term( "NaturalNumber" );

constexpr ctpg::nterm< FactorType > factor( "Factor" );
constexpr ctpg::nterm< FactorType > sum( "Sum" );
constexpr ctpg::nterm< FactorType > parenthesis_scope( "ParenthesisScope" );
constexpr ctpg::

// This function was "yoiked" directly from https://github.com/peter-winter/ctpg //
constexpr size_t to_size_t( std::string_view integer_token )
{
    size_t sum = 0;
    for( auto digit : integer_token )
        sum = ( sum * 10 ) + digit - '0';
    return sum;
}

constexpr ctpg::char_term plus_term( 
        static_cast< char >( ExpressionOperators::SumAdd ), 
        static_cast< size_t >( 1 ) , ctpg::associativity::ltor );
constexpr ctpg::char_term minus_term( 
        static_cast< char >( ExpressionOperators::SumSubtract ), 
        static_cast< size_t >( 1 ), ctpg::associativity::ltor );
constexpr ctpg::char_term multiply_term( 
        static_cast< char >( ExpressionOperators::FactorMultiply ), 
        static_cast< size_t >( 2 ), ctpg::associativity::ltor );
constexpr ctpg::char_term divide_term( 
        static_cast< char >( ExpressionOperators::FactorDivide ), 
        static_cast< size_t >( 2 ), ctpg::associativity::ltor );
constexpr ctpg::char_term left_parenthesis_term( '(', static_cast< size_t >( 3 ), ctpg::associativity::ltor );
constexpr ctpg::char_term right_parenthesis_term( ')', static_cast< size_t >( 3 ), ctpg::associativity::ltor );


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
        ctpg::nterms( factor, sum, parenthesis_scope ), 
        ctpg::rules( 
                factor( natural_number_term ) >= to_size_t, 
                factor( factor, multiply_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token ) {
                                return LiteralFactor{ current_factor * to_size_t( next_token ) }; 
                            }, 
                factor( factor, divide_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token ) {
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
            std::cout << "Result: " << std::get< size_t >( parse_result.value().factor ) << "\n";
        else
            std::cerr << "Error failed to parse!\n";
        std::cout << "Enter prompt please: ";
    }
    return 0;
}

/*#include <Warp/Expression.hpp>

// using NaturalNumberConstantExpression = ConstantExpression< size_t >;

auto parser = ctpg::parser{


}



constexpr static auto warp_parser = ctpg::parser

int main( int argc, char** args )
{
    const auto& factor_parser = //NaturalNumberConstantExpression::parser;
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
*/
