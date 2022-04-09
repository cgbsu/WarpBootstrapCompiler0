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

template< typename... ArithmaticParameterTypes >
struct StrongFactor
{
    using FactorType = std::variant< ArithmaticParameterTypes... >;
    using ThisType = StrongFactor< ArithmaticParameterTypes... >;
    FactorType factor;
    
    constexpr StrongFactor( FactorType factor ) : factor( factor ) {}
    constexpr StrongFactor( auto factor ) : factor( factor ) {}

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
    constexpr bool operator<=>( const auto other ) {
        return factor.operator<=>( other );
    }
};

using LiteralType = StrongFactor< size_t, long long int, double >;

enum class NodeType
{
    Factor, 
    Sum, 
    Literal 
};

template< auto >
struct Node {};

// I dont remember this being such a problem https://stackoverflow.com/questions/652155/invalid-use-of-incomplete-type

template< typename VariantParameterType >
struct LeftRight
{
    using VariantType = VariantParameterType;
    const VariantType left;
    const VariantType right;
    LeftRight( const VariantType left, const VariantType right ) 
            : left( left ), right( right ) {}
};

template<>
struct Node< NodeType::Literal >
{
    const LiteralType value;
    Node( LiteralType value ) : value( value ) {}
    Node( auto value ) : value( LiteralType{ value } ) {}
};

template< auto... VariantAlternativeParameterConstants >
struct Node< NodeType::Factor > : public LeftRight< 
        std::variant< Node< VariantAlternativeParameterConstants >... > 
    >
{
    using BaseType = LeftRight< 
            std::variant< Node< VariantAlternativeParameterConstants >... > 
        >
    const ExpressionOperator operation;
    Node( 
            const BaseType::VariantType left, 
            const ExpressionOperator operation, 
            const BaseType::VariantType right 
        ) : BaseType( left, right ), operation( operation ) {}
    // Node( Node< NodeType::Factor > const& other ) : left( other.left ), right( other.right ) {}
    

};

template< auto... VariantAlternativeParameterConstants >
struct Node< NodeType::Sum > : public LeftRight< 
        std::variant< Node< VariantAlternativeParameterConstants >... > 
    >
{
    using BaseType = LeftRight< 
            std::variant< Node< VariantAlternativeParameterConstants >... > 
        >
    const ExpressionOperator operation;
    Node( 
            const BaseType::VariantType left, 
            const ExpressionOperator operation, 
            const BaseType::VariantType right 
        ) : BaseType( left, right ), operation( operation ) {}

    constexpr operator Node< NodeType::Factor, DerivedParameterType >()
    {
        const BaseType* base = static_cast< BaseType* >( this );
        return Node< NodeType::Factor, DerivedParameterType >( base->left, base->right, operation );
    }
};


MulE() <- MulX( x ) <- Mul( x, y )

// This function was "yoiked" directly from https://github.com/peter-winter/ctpg //
constexpr size_t to_size_t( std::string_view integer_token )
{
    size_t sum = 0;
    for( auto digit : integer_token )
        sum = ( sum * 10 ) + digit - '0';
    return sum;
}

constexpr char char_cast( ExpressionOperator operation ) {
    return static_cast< char >( operation );
}

constexpr ctpg::nterm< Node< NodeType::Factor > > factor( "Factor" );
constexpr ctpg::nterm< Node< NodeType::Sum > > sum( "Sum" );
constexpr ctpg::nterm< Node< NodeType::Literal > > literal( "Literal" );

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
                literal
                //parenthesis_scope 
            ), 
        ctpg::rules( 
                literal( natural_number_term ) 
                        >= []( auto token ) {
                                return Node< NodeType::Literal >{ to_size_t( token ) };
                            }, 
                factor( factor, multiply_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token ) {
                                return Node< NodeType::Factor >{ current_factor, ExpressionOperator::FactorMultiply, next_token };
                            }, 
                factor( factor, divide_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token ) {
                                return Node< NodeType::Factor >{ current_factor, ExpressionOperator::FactorDivide, next_token };
                            }//,
                // parenthesis_scope( left_parenthesis_term, factor, right_parenthesis_term )
                        // >= [] ( auto, auto factor, auto ) { return factor; }, 
                // factor( parenthesis_scope ) >= []( auto parenthesis_scope ) { return parenthesis_scope; }, 
                // factor( factor, multiply_term, parenthesis_scope ) 
                //         >= []( auto factor, auto, auto parenthesis_scope ) { 
                //                 return factor * parenthesis_scope; 
                //             }, 
                // factor( factor, divide_term, parenthesis_scope ) 
                //         >= []( auto factor, auto, auto parenthesis_scope ) { 
                //                 return factor / parenthesis_scope; 
                //             }, 

                // factor( sum ) >= []( auto sum ) { return sum; }, 
                // sum( factor, plus_term, factor ) 
                //         >= []( auto current_sum, auto, const auto& next_token ) {
                //                 return  current_sum->add( std::move( next_token ) ); 
                //             }, 
                // sum( factor, minus_term, factor ) 
                //         >= []( auto current_sum, auto, const auto& next_token ) {
                //                 return current_sum->subtract( std::move( next_token ) ); 
                //             }
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
            std::cout << "Result: " << std::visit( []( auto data ) { return data.to_string(); }, parse_result.value() ) << "\n";
        else
            std::cerr << "Error failed to parse!\n";
        std::cout << "Enter prompt please: ";
    }
    return 0;
}
