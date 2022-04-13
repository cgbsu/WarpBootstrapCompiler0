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
#include <ThirdParty/constexpr_unique_ptr.hpp>

/* See: https://github.com/cplusplus/papers/issues/961 *****************************
** and http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2273r0.pdf *********
** https://github.com/riscygeek/constexpr_suff/blob/master/include/unique_ptr.hpp */

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

/*
Starting verison
template<class T, class U = T>
constexpr // since C++20
T exchange(T& obj, U&& new_value)
    noexcept( // since C++23
        std::is_nothrow_move_constructible<T>::value &&
        std::is_nothrow_assignable<T&, U>::value
    )
{
    T old_value = std::move(obj);
    obj = std::forward<U>(new_value);
    return old_value;
}*/

template<class T, class U = T>
constexpr // since C++20
T& exchange(T& obj, U&& new_value)
    noexcept( // since C++23
        std::is_nothrow_move_constructible<T>::value &&
        std::is_nothrow_assignable<T&, U>::value
    )
{
    T& old_value = std::move(obj);
    obj = std::forward<U>(new_value);
    return old_value;
}

/*
  // C++11 version of std::exchange for internal use.
  template <typename _Tp, typename _Up = _Tp>
    _GLIBCXX20_CONSTEXPR
    inline _Tp
    __exchange(_Tp& __obj, _Up&& __new_val)
    {
      _Tp __old_val = std::move(__obj);
      __obj = std::forward<_Up>(__new_val);
      return __old_val;
    }


  /// Assign @p __new_val to @p __obj and return its previous value.
  template <typename _Tp, typename _Up = _Tp>
    _GLIBCXX20_CONSTEXPR
    inline _Tp
    exchange(_Tp& __obj, _Up&& __new_val)
    { return std::__exchange(__obj, std::forward<_Up>(__new_val)); }
*/
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


struct ConstexprStringable
{
    constexpr virtual std::string_view to_string() const = 0;
    constexpr operator std::string_view() const {
        return to_string();
    }
};

/*
template< 
        auto OperationParameterConstant, 
        typename CurrentPossibleParameterType, 
        typename... PossibleParameterTypes 
    >
constexpr auto operate_if( auto variant ) -> decltype( std::function{ OperationParameterConstant } )::result_type
{
    if constexpr( 
            CurrentPossibleParameterType* data = std::get_if< CurrentPossibleParameterType >( variant ); 
            data != nullptr
        ) {
        return OperationParameterConstant( data );
    }
    else {
        return operate_if< OperationParameterConstant, PossibleParameterTypes... >( variant );
    }
}
*/
template< typename... ArithmaticParameterTypes >
struct StrongFactor// : public ConstexprStringable
{
    using FactorType = std::variant< ArithmaticParameterTypes... >;
    using ThisType = StrongFactor< ArithmaticParameterTypes... >;
    FactorType factor;
    
    constexpr StrongFactor( FactorType factor ) : factor( factor ) {}
    constexpr StrongFactor( auto factor ) : factor( factor ) {}
/*
    constexpr virtual std::string_view to_string() const override final
    {
        return operate_if< 
                []( auto data ) -> std::string_view { return to_string( data ); }, 
                ArithmaticParameterTypes... 
            >( factor );
    }
*/
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

struct NodePointer : public Owner
{
    using PointerType = const Node*;
    const PointerType pointer;
    constexpr NodePointer( const PointerType other_pointer ) : pointer( other_pointer ) {}
    constexpr NodePointer( const NodePointer& other ) : pointer( other.pointer ) {}

    template< typename NodeType >
    constexpr static NodePointer make_pointer( auto... constructor_values ) {
        return NodePointer( new NodeType{ constructor_values... } );
    }

    auto operator->() const {
        return pointer;
    }

    constexpr virtual bool clean_up() const override final
    {
        const bool status = pointer->clean_up();
        delete pointer;
        return status;
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
    constexpr virtual bool clean_up() const override final {
        return left->clean_up() && right->clean_up();
    }
};

struct LiteralNode : public BaseNode< NodeType::Literal >
{
    const LiteralType value;
    constexpr LiteralNode( const LiteralType& value ) : value( value ) {}
    // constexpr LiteralNode( auto value ) : value( LiteralType{ value } ) {}
    constexpr LiteralNode( LiteralNode const& other ) : value( other.value ) {}
    // constexpr virtual const NodePointer duplicate() const override final
    // {
    //     return NodePointer{ .pointer =  benni::make_unique< LiteralNode >( value ) };
    // }
    constexpr virtual bool clean_up() const override final {
        return true;
    }
    constexpr static const char* test = "abc";
    constexpr virtual std::string_view to_string() const override final
    {
        /*return std::visit( [ & ]( auto data_value ) { 
                return to_string< decltype( data_value ) >( data_value );
            }, value.factor );*/
        return std::string_view{ test };
    }
};


template< auto OperationParameterConstant >
struct OperationNode : public LeftRightNode< OperationParameterConstant >
{
    using BaseType = LeftRightNode< OperationParameterConstant >;
    constexpr OperationNode( const NodePointer left, const NodePointer right ) : 
            LeftRightNode< OperationParameterConstant >( left, right ) {}
    constexpr static const auto operation = OperationParameterConstant;
    using OperationType = decltype( operation );
    constexpr virtual std::string_view to_string() const override final
    {
        std::stringstream buffer;
        buffer 
                << BaseType::left->to_string() 
                << " " 
                << to_string< OperationType >( operation ) 
                << " " 
                << BaseType::right->to_string();
        return std::string_view{ buffer.str() };
    }
};

#define DEFINE_OPERATION_NODE( NODE_NAME, OPERATION_TYPE ) \
    struct NODE_NAME : public OperationNode< OPERATION_TYPE > \
    { \
        constexpr NODE_NAME ( \
                const NodePointer left, \
                const NodePointer right \
            ) : OperationNode< OPERATION_TYPE >( left, right ) {} \
        constexpr NODE_NAME( NODE_NAME const& other ) : \
                        OperationNode< OPERATION_TYPE >( other.left, other.right ) {} \
    }
    //     constexpr virtual const NodePointer duplicate() const override final \
    //     { \
    //         return NodePointer{ static_cast< NodePointer::PointerType >( \
    //                 benni::make_unique< const NODE_NAME >( left, right ) \
    //             ) }; \
    //     } \
    // }

DEFINE_OPERATION_NODE( MultiplyNode, ExpressionOperator::FactorMultiply );
DEFINE_OPERATION_NODE( DivideNode, ExpressionOperator::FactorDivide );
DEFINE_OPERATION_NODE( AddNode, ExpressionOperator::SumAdd );
DEFINE_OPERATION_NODE( SubtractNode, ExpressionOperator::SumSubtract );

using FactorNodeType = std::variant< MultiplyNode, DivideNode >;
using SumNodeType = std::variant< AddNode, SubtractNode >;


constexpr char char_cast( ExpressionOperator operation ) {
    return static_cast< char >( operation );
}

constexpr static ctpg::nterm< FactorNodeType > factor{ "Factor" };
constexpr static ctpg::nterm< SumNodeType > sum{ "Sum" };
constexpr static ctpg::nterm< FactorNodeType > parenthesis_scope{ "ParenthesisScope" };

constexpr static char natural_number_regex[] = "[0-9][0-9]*";
constexpr static ctpg::regex_term< natural_number_regex > natural_number_term{ "NaturalNumber" };
/*
constexpr static ctpg::char_term plus_term( 
        ExpressionOperator::SumAdd, 1, ctpg::associativity::ltor 
    );
constexpr static ctpg::char_term minus_term( 
        ExpressionOperator::SumSubtract, 1, ctpg::associativity::ltor 
    );
constexpr static ctpg::char_term multiply_term( 
        ExpressionOperator::FactorMultiply, 2, ctpg::associativity::ltor 
    );
constexpr static ctpg::char_term divide_term( 
        ExpressionOperator::FactorDivide, 2, ctpg::associativity::ltor 
    );
constexpr static ctpg::char_term left_parenthesis_term( '(', 3, ctpg::associativity::ltor );
constexpr static ctpg::char_term right_parenthesis_term( ')', 3, ctpg::associativity::ltor );
*/
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
                                return VariantType{ benni::make_unique< const Node< NodeType::Literal > >( to_size_t( token ) ) };
                            }, 
                factor( factor, multiply_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token )
                            {
                                using Type = Node< NodeType::Factor >;
                                return VariantType{ benni::make_unique< Type > ( 
                                        current_factor, 
                                        ExpressionOperator::FactorMultiply, 
                                        VariantType{ benni::make_unique< const Node< NodeType::Literal > >( to_size_t( token ) ) } 
                                    ) };
                            }, 
                factor( factor, divide_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token )
                            {
                                using Type = Node< NodeType::Factor >;
                                return VariantType{ benni::make_unique< Type > ( 
                                        current_factor, 
                                        ExpressionOperator::FactorDivide, 
                                        VariantType{ benni::make_unique< const Node< NodeType::Literal > >( to_size_t( token ) ) } 
                                    ) };
                            }, 
                parenthesis_scope( left_parenthesis_term, factor, right_parenthesis_term )
                        >= [] ( auto, auto factor, auto ) { return factor; }, 
                factor( parenthesis_scope ) >= []( auto parenthesis_scope ) { return parenthesis_scope; }, 
                factor( factor, multiply_term, parenthesis_scope ) 
                        >= []( auto factor, auto, auto parenthesis_scope ) 
                            {
                                using Type = Node< NodeType::Factor >;
                                return VariantType{ benni::make_unique< Type > ( 
                                        factor, 
                                        ExpressionOperator::FactorMultiply, 
                                        parenthesis_scope 
                                    ) };
                            }, 
                factor( factor, divide_term, parenthesis_scope ) 
                        >= []( auto factor, auto, auto parenthesis_scope ) 
                            {
                                using Type = Node< NodeType::Factor >;
                                 return VariantType{ benni::make_unique< Type > ( 
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
                                return VariantType{ benni::make_unique< Type > ( 
                                        current_sum, 
                                        ExpressionOperator::SumAdd, 
                                        next_token 
                                    ) };
                            }, 
                sum( factor, minus_term, factor ) 
                        >= []( auto current_sum, auto, const auto& next_token )
                            {
                                using Type = Node< NodeType::Sum >;
                                return VariantType{ benni::make_unique< Type > ( 
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
