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

enum class NodeType : char
{
    Factor = 'F', 
    Sum = 'S', 
    Literal = 'L' 
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

struct Taggable
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
struct BaseNode : public Taggable
{
    protected: 
        constexpr virtual const std::any type_tag() const override final {
            return TagParameterConstant;
        }
};

template< auto >
struct Node {};


struct Node< ExpressionOperator::FactorMultiply >;
struct Node< ExpressionOperator::FactorDivide >;
struct Node< ExpressionOperator::SumAdd >;
struct Node< ExpressionOperator::SumSubtract >;
struct Node< NodeType::Literal >;

using VariantType = std::variant< 
        const Node< ExpressionOperator::FactorMultiply >*, 
        const Node< ExpressionOperator::FactorDivide >*, 
        const Node< ExpressionOperator::SumAdd >*, 
        const Node< ExpressionOperator::SumSubtract >*, 
        const Node< NodeType::Literal >* 
    >;

/*struct VariantType
{
    const Taggable* node;
    constexpr VariantType() : node( nullptr ) {}
    constexpr VariantType( const Taggable* node ) : node( node ) {}
    constexpr VariantType( const VariantType& other ) = default;
    constexpr VariantType( VariantType&& other ) = default;
    constexpr VariantType& operator=( const VariantType& other ) = default;
    constexpr VariantType& operator=( VariantType&& other ) = default;

    constexpr const Taggable* operator->() const {
        return node;
    }
};*/

template< auto TagParameterConstant >
struct LeftRight : public BaseNode< TagParameterConstant >
{
    const VariantType left;
    const VariantType right;
    constexpr LeftRight( const VariantType left, const VariantType right ) 
            : left( left ), right( right ) {}    
};

template<>
struct Node< NodeType::Literal > : public BaseNode< NodeType::Literal >
{
    const LiteralType value;
    constexpr Node( LiteralType value ) : value( value ) {}
    constexpr Node( auto value ) : value( value ) {}
    constexpr Node( Node< NodeType::Literal > const& other ) : value( other.value ) {}
};


#define DEFINE_BI_NODE( VALUE ) \
    template<> \
    struct Node< VALUE > : public LeftRight< VALUE > \
    { \
        using BaseType = LeftRight< VALUE >; \
        constexpr static const ExpressionOperator operation = VALUE ; \
        constexpr Node( \
                const VariantType left, \
                const VariantType right \
            ) : BaseType( left, right ) {} \
        constexpr Node( Node< VALUE > const& other ) \
                : BaseType( other.left, other.right ) {} \
    }

DEFINE_BI_NODE( ExpressionOperator::FactorMultiply );
DEFINE_BI_NODE( ExpressionOperator::FactorDivide );
DEFINE_BI_NODE( ExpressionOperator::SumAdd );
DEFINE_BI_NODE( ExpressionOperator::SumSubtract );

template< auto NodeTag >
constexpr VariantType allocate_node( auto... constructor_arguments )
{
    return VariantType{ 
            new Node< NodeTag >{ constructor_arguments... }
        };
}

template< typename LiteralType >
constexpr VariantType allocate_integral_literal_node( auto data )
{
    return VariantType{ 
            new Node< NodeType::Literal >{ 
                    Utility::to_integral< LiteralType >( data ) 
                }
        };
}

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
                                return allocate_integral_literal_node< size_t >( token );
                            }, 
                factor( factor, multiply_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token )
                            {
                                return allocate_node< ExpressionOperator::FactorMultiply >( 
                                        current_factor, 
                                        allocate_integral_literal_node< size_t >( next_token )
                                    );
                            }, 
                factor( factor, divide_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token )
                            {
                                return allocate_node< ExpressionOperator::FactorDivide >( 
                                        current_factor, 
                                        allocate_integral_literal_node< size_t >( next_token )
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

template< typename = void >
struct StringableTrait
{
    virtual std::string_view to_string( const size_t tabs ) const = 0;
    std::string_view make_tabs( const size_t tabs ) const
    {
        std::stringstream tab_buffer;
        for( size_t ii = 0; ii < tabs; ++ii )
            tab_buffer << "\t";
        return std::string_view{ tab_buffer.str() };
    }
};

// struct StringableTrait< const Node< ExpressionOperator::FactorMultiply > >;  
// struct StringableTrait< const Node< ExpressionOperator::FactorDivide > >; 
// struct StringableTrait< const Node< ExpressionOperator::SumAdd > >; 
// struct StringableTrait< const Node< ExpressionOperator::SumSubtract > >; 
// struct StringableTrait< const Node< NodeType::Literal > >; 
// struct StringableTrait< VariantType >; 

std::string_view tree_to_string( const VariantType node, const size_t tabs );

#define DEFINE_STRINGABLE_FOR_BI_NODE_TYPE( TAG_VALUE ) \
    template<> \
    struct StringableTrait< const Node< TAG_VALUE >* > : public StringableTrait< void > \
    { \
        const Node< TAG_VALUE >* node; \
        constexpr StringableTrait( const Node< TAG_VALUE >* node ) : node( node ) {} \
        virtual std::string_view to_string( const size_t tabs_ ) const override final \
        { \
            std::string_view child_nodes[] = { \
                    tree_to_string( node->left, tabs_ + 1 ), \
                    tree_to_string( node->right, tabs_ + 1 ) \
                }; \
            std::string_view tabs = make_tabs( tabs_ ); \
            std::stringstream buffer; \
            buffer \
                << tabs << "Node< " << Utility::to_string< decltype( TAG_VALUE ) >( TAG_VALUE ) << ">{\n" \
                << tabs << "\t" << "Left{\n" << child_nodes[ 0 ] \
                << tabs << "\t}\n" \
                << tabs << "\t" << "Right{\n" \
                << tabs << "\t" << child_nodes[ 1 ] \
                << tabs << "\t}\n" \
                << tabs << "}\n"; \
            return std::string_view{ buffer.str() }; \
        } \
    }



DEFINE_STRINGABLE_FOR_BI_NODE_TYPE( ExpressionOperator::FactorMultiply );
DEFINE_STRINGABLE_FOR_BI_NODE_TYPE( ExpressionOperator::FactorDivide );
DEFINE_STRINGABLE_FOR_BI_NODE_TYPE( ExpressionOperator::SumAdd );
DEFINE_STRINGABLE_FOR_BI_NODE_TYPE( ExpressionOperator::SumSubtract );

template<>
struct StringableTrait< const Node< NodeType::Literal >* > : public StringableTrait< void >
{
    const Node< NodeType::Literal >* node;
    constexpr StringableTrait( const Node< NodeType::Literal >* node ) : node( node ) {}
    virtual std::string_view to_string( const size_t tabs ) const override final 
    {
        std::stringstream buffer;
        buffer 
                << make_tabs( tabs ) 
                << "Node< " << Utility::to_string< NodeType >( NodeType::Literal ) 
                << ">{" << node->value.to_string() << "}\n";
        return std::string_view{ buffer.str() };
    }
};


std::string_view tree_to_string( const VariantType node, const size_t tabs )
{
    StringableTrait< void >* stringable = nullptr;
    std::visit( [ & ]( auto underyling_node ) {
            stringable = new StringableTrait< decltype( underyling_node ) >{ underyling_node };
        }, node );
    std::string_view string = stringable->to_string( tabs );
    delete stringable;
    return string;
};


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
            std::cout << "Result: " << tree_to_string( parse_result.value(), 0 ) << "\n";
        else
            std::cerr << "Error failed to parse!\n";
        std::cout << "Enter prompt please: ";
    }
    return 0;
}
