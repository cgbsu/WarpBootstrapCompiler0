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

struct ConstexprStringable
{
    constexpr virtual const char* to_string() const = 0;
    constexpr operator const char*() const {
        return to_string();
    }
};

/*
template< typename... AlternativeParameterTypes >
struct PointerVariant
{
    using VariantType = std::variant< std::unique_ptr< AlternativeParameterTypes >... >;
    const VariantType pointer_variant;

    template< typename HeldParameterType >
    std::unique_ptr< HeldParameterType > pass_through( std::unique_ptr< HeldParameterType > pointer )
    {
    }

    PointerVariant( std::unique_ptr< auto > pointer ) : pointer_variant( VariantType{ pointer } ) {}
    PointerVariant( std::unique_ptr< auto > pointer ) : pointer_variant( VariantType{ pointer } ) {}
};
*/


template< auto >
struct Node {};

// I dont remember this being such a problem https://stackoverflow.com/questions/652155/invalid-use-of-incomplete-type

struct Node< NodeType::Factor >;
struct Node< NodeType::Sum >;
struct Node< NodeType::Literal >;


template< auto NodeTypeParameterConstant >
struct NodePointer
{
    using NodeType = Node< NodeTypeParameterConstant >;
    using PointerType = std::unique_ptr< const NodeType >;
    using ThisType = NodePointer< NodeTypeParameterConstant >;
    PointerType pointer;
    constexpr NodePointer( PointerType pointer ) : pointer( pointer ) {}
    constexpr NodePointer( NodePointer const& other ) : pointer( other.transfer() ) {}
    constexpr PointerType transfer() {
        return pointer;
    }
};

template< auto NodeTypeParameterConstant >
NodePointer< NodeTypeParameterConstant > make_node_pointer( auto... constructor_values ) {
    return NodePointer{ std::make_unique< const Node< NodeTypeParameterConstant > >( constructor_values... ) };
}

struct VariantTypeHolder
{
    using VariantType = std::variant< 
        NodePointer< NodeType::Factor >, 
        NodePointer< NodeType::Sum >, 
        NodePointer< NodeType::Literal > 
    >;
};

struct Duplicator : public VariantTypeHolder {
    using VariantType = VariantTypeHolder::VariantType;
    constexpr virtual const VariantType duplicate() = 0;
};

struct Owner {
    // constexpr virtual bool deallocate() = 0;
};


struct LeftRight : public Duplicator, public Owner, public ConstexprStringable
{
    using VariantType = Duplicator::VariantType;
    const VariantType left;
    const VariantType right;
    constexpr LeftRight( const VariantType left, const VariantType right ) 
            : left( left ), right( right ) {}
    // constexpr virtual bool deallocate() override final
    // {
    //     auto allocated = [ & ]( auto side ) {
    //             return std::holds_alternative< Owner >( side ); //Node< NodeType::Factor >* >( left ) 
    //                     // || std::holds_alternative< Node< NodeType::Sum >* >( left );
    //         };
    //     if( allocated( left ) == true )
    //     {
    //         std::visit( []( LeftRight* left_data ) {
    //                 // static_cast< LeftRight* >( left_data )->deallocate();
    //                 // delete left_data;
    //                 // return nullptr;
    //             }, left );
    //     }
    //     if( allocated( right ) == true )
    //     {
    //         std::visit( []( LeftRight* right_data ) {
    //                 // static_cast< LeftRight* >( right_data )->deallocate();
    //                 // delete left_data;
    //                 // return nullptr;
    //             }, right );
    //     }
    //     return true;
    // }
};

template<> 
struct Node< NodeType::Literal > : public Duplicator, public Owner, public ConstexprStringable
{
    using VariantType = Duplicator::VariantType;
    const LiteralType value;
    constexpr Node( LiteralType value ) : value( value ) {}
    constexpr Node( auto value ) : value( LiteralType{ value } ) {}
    constexpr Node( Node< NodeType::Literal > const& other ) : value( other.value ) {}
    constexpr virtual const VariantType duplicate() override final {
        return VariantType{ std::make_unique< const Node< NodeType::Literal > >( value ) };
    }
        constexpr static const char* debug = "Literal";
    constexpr virtual const char* to_string() const override final {
        return debug;
    }
    // constexpr virtual bool deallocate() override final {
    //     return true;
    // }
};

template<>
struct Node< NodeType::Factor > : public LeftRight
{
    using BaseType = LeftRight;
    using VariantType = BaseType::VariantType;
    const ExpressionOperator operation;
    constexpr Node( 
            const VariantType left, 
            const ExpressionOperator operation, 
            const VariantType right 
        ) : BaseType( left, right ), operation( operation ) {}
    constexpr Node( Node< NodeType::Factor > const& other ) 
            : BaseType( other.left, other.right ), 
                    operation( other.operation ) {}
    constexpr virtual const VariantType duplicate() override final {
        return VariantType{ std::make_unique< const Node< NodeType::Factor > >( left, operation, right ) };
    }
        constexpr static const char* debug = "Factor";
    constexpr virtual const char* to_string() const override final {
        return debug;
    }
};

template<>
struct Node< NodeType::Sum > : public LeftRight
{
    using BaseType = LeftRight;
    const ExpressionOperator operation;
    using VariantType = BaseType::VariantType;
    constexpr Node( 
            const VariantType left, 
            const ExpressionOperator operation, 
            const VariantType right 
        ) : BaseType( left, right ), operation( operation ) {}
    constexpr Node( Node< NodeType::Sum > const& other ) 
            : BaseType( other.left, other.right ), 
                    operation( other.operation ) {}
    constexpr virtual const VariantType duplicate() override final {
        return VariantType{ std::make_unique< const Node< NodeType::Sum > >( left, operation, right ) };
    }
        constexpr static const char* debug = "Sum";
    constexpr virtual const char* to_string() const override final {
        return debug;
    }

};

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

using VariantType = Node< NodeType::Factor >::VariantType;

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
