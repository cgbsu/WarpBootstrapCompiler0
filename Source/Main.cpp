#include <iostream>
#include <string>
#include <string_view>
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

template< std::integral ParameterType >
constexpr auto to_integral( std::string_view integer_token )
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

template< typename ParameterType >
concept Enumaration = std::is_enum< ParameterType >::value;

// template< Enumaration EnumerationParameterType >
// constexpr std::string_view to_string( EnumerationParameterType to_stringify )
// {
//     return std::string_view{ std::string{ 
//             static_cast< std::underlying_type_t( to_stringify ) >( to_stringify ) 
//         } };
// }

template< std::integral IntegralParameterType >
constexpr std::string_view to_string( IntegralParameterType to_stringify ) {
    return std::string_view{ std::atoi( to_stringify ) };
}

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

using LiteralType = StrongFactor< size_t, long long int >;

enum class NodeType
{
    Factor, 
    Sum, 
    Literal 
};

struct ConstexprStringable
{
    constexpr virtual std::string_view to_string() const = 0;
    constexpr operator std::string_view() const {
        return to_string();
    }
};

struct NodePointer;

struct Duplicator {
    constexpr virtual const NodePointer duplicate() const = 0;
};

struct Owner {
    constexpr virtual bool clean_up() const = 0;
};

struct Node : public ConstexprStringable, public Duplicator, public Owner
{
    template< typename NodeTagTypeParameterType >
    constexpr const std::optional< NodeTagTypeParameterType > tag_as() const
    {
        if constexpr( 
                    auto result = std::any_cast< NodeTagTypeParameterType >( type_tag ); 
                    result != nullptr 
                )
            return std::optional{ result };
        return std::nullopt;
    }
    // Includes runtime check, cant do it at compile time :( //
    template< typename NodeTagTypeParameterType >
    const std::optional< NodeTagTypeParameterType > safe_tag_as() const
    {
        if( tag_type_id() == typeid( NodeTagTypeParameterType ).hash_code() )
            return tag_as< NodeTagTypeParameterType >();
        return std::nullopt;
    }
    // Runtime function //
    size_t tag_type_id() const {
        return type_tag().type().hash_code();
    }
    protected: 
        constexpr virtual const std::any type_tag() const = 0;
};

template< auto TagParameterConstant >
struct BaseNode : public Node
{
    protected: 
        constexpr virtual const std::any type_tag() const override final {
            return TagParameterConstant;
        }
};

struct NodePointer
{
    using PointerType = std::unique_ptr< Node >;
    PointerType pointer;
    constexpr NodePointer( PointerType& other_pointer )
            : pointer( std::exchange( other_pointer, nullptr ) ) {}
    constexpr NodePointer( NodePointer& other ) 
            : pointer( std::exchange( other.pointer, nullptr ) ) {}
    auto operator->() const {
        return pointer.operator->();
    }
};

template< auto NodeTypeParameterConstant >
struct LeftRightNode : public BaseNode< NodeTypeParameterConstant >
{
    using ThisType = LeftRightNode< NodeTypeParameterConstant >;
    const NodePointer left;
    const NodePointer right;
    constexpr LeftRightNode( const NodePointer left, const NodePointer right ) 
            : left( left ), right( right ) {}
    constexpr virtual bool clean_up() override final {
        return left->clean_up() && right->clean_up();
    }
};

struct LiteralNode : public BaseNode< NodeType::Literal >
{
    const LiteralType value;
    constexpr Node( LiteralType value ) : value( value ) {}
    constexpr Node( auto value ) : value( LiteralType{ value } ) {}
    constexpr Node( Node< NodeType::Literal > const& other ) : value( other.value ) {}
    constexpr virtual const VariantType duplicate() override final {
        return NodePointer{ std::make_unique< const Node< NodeType::Literal > >( value ) };
    }
    constexpr virtual bool clean_up() override final {
        return true;
    }
    constexpr virtual std::string_view to_string() const override final
    {
        return std::visit( [ & ]( auto data_value ) { 
                return to_string< decltype( data_value ) >( data_value );
            }, value.factor );
    }
};


template< auto OperationParameterConstant >
struct OperationNode : public LeftRightNode< OperationParameterConstant >
{
    constexpr OperationNode( const NodePointer left, const NodePointer right ) : 
            LeftRightNode< OperationParameterConstant >( left, right ) {}
    constexpr const auto operation = OperationParameterConstant;
    using OperationType = decltype( operation );
    constexpr virtual std::string_view to_string() const override final
    {
        std::stringstream buffer;
        buffer 
                << left->to_string() 
                << " " 
                << to_string< OperationType >( operation ) 
                << " " 
                << right->to_string();
        return std::string_view{ buffer.str() };
    }

}

#define DEFINE_OPERATION_NODE( NODE_NAME, OPERATION_TYPE ) \
    struct NODE_NAME : public OperationNode< OPERATION_TYPE > \
    { \
        constexpr NODE_NAME ( \
                const NodePointer left, \
                const NodePointer right \
            ) : public OperationNode< OPERATION_TYPE >( left, right ) {} \
        constexpr NODE_NAME( NODE_NAME const& other ) : \
                        public OperationNode< OPERATION_TYPE >( other.left, other.right ), \
                        operation( other.operation ) {} \
        constexpr virtual const NodePointer duplicate() override final { \
            return NodePointer{ std::make_unique< const NODE_NAME >( left, right ) }; \
        } \
    }

DEFINE_OPERATION_NODE( MultiplyNode, ExpressionOperator::FactorMultiply );
DEFINE_OPERATION_NODE( DivideNode, ExpressionOperator::FactorDivide );
DEFINE_OPERATION_NODE( AddNode, ExpressionOperator::SumAdd );
DEFINE_OPERATION_NODE( SubtractNode, ExpressionOperator::SumSubtract );

using FactorNodeType = std::variant< MultiplyNode, DivideNode >;
using SumNodeType = std::variant< SumNode, SubtractNode >;


constexpr char char_cast( ExpressionOperator operation ) {
    return static_cast< char >( operation );
}

constexpr ctpg::nterm< FactorNodeType > factor( "Factor" );
constexpr ctpg::nterm< SumNodeType > sum( "Sum" );
constexpr ctpg::nterm< FactorNodeType > parenthesis_scope( "ParenthesisScope" );

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

/*
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
                                return VariantType{ std::make_unique< const Node< NodeType::Literal > >( to_size_t( token ) ) };
                            }, 
                factor( factor, multiply_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token )
                            {
                                using Type = Node< NodeType::Factor >;
                                return VariantType{ std::make_unique< Type > ( 
                                        current_factor, 
                                        ExpressionOperator::FactorMultiply, 
                                        VariantType{ std::make_unique< const Node< NodeType::Literal > >( to_size_t( token ) ) } 
                                    ) };
                            }, 
                factor( factor, divide_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token )
                            {
                                using Type = Node< NodeType::Factor >;
                                return VariantType{ std::make_unique< Type > ( 
                                        current_factor, 
                                        ExpressionOperator::FactorDivide, 
                                        VariantType{ std::make_unique< const Node< NodeType::Literal > >( to_size_t( token ) ) } 
                                    ) };
                            }, 
                parenthesis_scope( left_parenthesis_term, factor, right_parenthesis_term )
                        >= [] ( auto, auto factor, auto ) { return factor; }, 
                factor( parenthesis_scope ) >= []( auto parenthesis_scope ) { return parenthesis_scope; }, 
                factor( factor, multiply_term, parenthesis_scope ) 
                        >= []( auto factor, auto, auto parenthesis_scope ) 
                            {
                                using Type = Node< NodeType::Factor >;
                                return VariantType{ std::make_unique< Type > ( 
                                        factor, 
                                        ExpressionOperator::FactorMultiply, 
                                        parenthesis_scope 
                                    ) };
                            }, 
                factor( factor, divide_term, parenthesis_scope ) 
                        >= []( auto factor, auto, auto parenthesis_scope ) 
                            {
                                using Type = Node< NodeType::Factor >;
                                 return VariantType{ std::make_unique< Type > ( 
                                       factor, 
                                        ExpressionOperator::FactorDivide, 
                                        parenthesis_scope 
                                    ) };
                            }, 
                factor( sum ) 
                        >= []( auto sum ) {
                            return VariantType{ sum };
                        }, 
                sum( factor, plus_term, factor ) 
                        >= []( auto current_sum, auto, const auto& next_token ) 
                            {
                                using Type = Node< NodeType::Sum >;
                                return VariantType{ std::make_unique< Type > ( 
                                        current_sum, 
                                        ExpressionOperator::SumAdd, 
                                        next_token 
                                    ) };
                            }, 
                sum( factor, minus_term, factor ) 
                        >= []( auto current_sum, auto, const auto& next_token )
                            {
                                using Type = Node< NodeType::Sum >;
                                return VariantType{ std::make_unique< Type > ( 
                                        current_sum, 
                                        ExpressionOperator::SumSubtract, 
                                        next_token 
                                    ) };
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
*/

int main() {
    return 0;
}
