#include <iostream>
#include <string>
#include <sstream>
#include <concepts>
#include <variant>
#include <type_traits>
#include <utility>
#include <memory>
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

enum class OtherNodeType : size_t
{
    FactorStem = 0 
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


template< typename... ArithmaticParameterTypes >
struct WeakFactor
{
    using FactorType = std::variant< ArithmaticParameterTypes... >;
    using ThisType = WeakFactor< ArithmaticParameterTypes... >;
    FactorType factor;
    
    constexpr WeakFactor( FactorType factor ) : factor( factor ) {}

    constexpr ThisType operator*( const auto other ) {
        return ThisType{ std::visit( [ & ]( auto value ) { return value * other; }, factor ) };
    }
    constexpr ThisType operator/( const auto other ) {
        return ThisType{ std::visit( [ & ]( auto value ) { return value / other; }, factor ) };
    }
    constexpr ThisType operator+( const auto other ) {
        return ThisType{ std::visit( [ & ]( auto value ) { return value + other; }, factor ) };
    }
    constexpr ThisType operator-( const auto other ) {
        return ThisType{ std::visit( [ & ]( auto value ) { return value - other; }, factor ) };
    }
    constexpr bool operator<=>( const auto other ) {
        return factor.operator<=>( other );
    }
};

using FactorType = StrongFactor< size_t, int, float, double >;

struct ToString
{
    virtual std::string to_string() const = 0;
    operator std::string() const {
        return to_string();
    }
};

struct BaseNode : public ToString
{
    using ThisType = BaseNode;
    using BaseNodeType = BaseNode;
    // Sweet sweet OOP, oh how I have missed you. //////
    // Im sorry I left you baby. Except Im not I just //
    // Gadda learn data oriented programming, but //////
    // Oh sweet sweet OOP, dynamic dispatch, Im gunna //
    // cry my eyes out later. //////////////////////////
    constexpr virtual BaseNodeType& multiply( BaseNodeType const& other ) = 0;
    constexpr virtual BaseNodeType& divide( BaseNodeType const& other ) = 0;
    constexpr virtual BaseNodeType& add( BaseNodeType const& other ) = 0;
    constexpr virtual BaseNodeType& subtract( BaseNodeType const& other ) = 0;
    BaseNodeType& operator*( BaseNodeType const& other ) {
        return multiply( other );
    }
    BaseNodeType& operator/( BaseNodeType const& other ) {
        return divide( other );
    }
    BaseNodeType& operator+( BaseNodeType const& other ) {
        return add( other );
    }
    BaseNodeType& operator-( BaseNodeType const& other ) {
        return subtract( other );
    }
    friend std::ostream& operator<<( std::ostream& output_media, const BaseNode& node ) {
        output_media << node.to_string();
        return output_media;
    }
};


using BaseNodeType = BaseNode;

template< typename ThisParameterType, template< ExpressionOperator > typename ExpressionParameterType >
struct MakeSuper : public BaseNode
{
    template< ExpressionOperator make_super_expressionOperationParameterConstant >
    BaseNodeType& make_super_expression( BaseNodeType const& other ) {
        BaseNodeType pointer = ExpressionParameterType< make_super_expressionOperationParameterConstant >( *this, other );
        return pointer;
    }

    constexpr virtual BaseNodeType& multiply( BaseNodeType const& other ) override final {
        return make_super_expression< ExpressionOperator::FactorMultiply >( other );
    }
    constexpr virtual BaseNodeType& divide( BaseNodeType const& other ) override final {
        return make_super_expression< ExpressionOperator::FactorDivide >( other );
    }
    constexpr virtual BaseNodeType& add( BaseNodeType const& other ) override final {
        return make_super_expression< ExpressionOperator::SumAdd >( other );
    }
    constexpr virtual BaseNodeType& subtract( BaseNodeType const& other ) override final {
        return make_super_expression< ExpressionOperator::SumSubtract >( other );
    }
};

template< ExpressionOperator OperationParameterConstant >
struct ExpressionNode : public MakeSuper< ExpressionNode< OperationParameterConstant >, ExpressionNode >
{
    constexpr static const ExpressionOperator operation = OperationParameterConstant;
    BaseNodeType& left;
    BaseNodeType& right;
    using ThisType = ExpressionNode< OperationParameterConstant >;

    constexpr ExpressionNode( 
                    BaseNodeType& left, 
                    BaseNodeType& right 
            ) : 
            left( left ), 
            right( right )
        {}
    constexpr ExpressionNode( ThisType const& other ) = default;
    constexpr ExpressionNode( ThisType&& other ) = default;
    virtual std::string to_string() const override
    {
        std::stringstream stream;
        std::string arguments = left.to_string() + ", " + right.to_string();
        if constexpr( OperationParameterConstant == ExpressionOperator::FactorMultiply )
            stream << "Multiply(" << arguments << ")";
        if constexpr( OperationParameterConstant == ExpressionOperator::FactorDivide )
            stream << "Divide(" << arguments << ")";
        if constexpr( OperationParameterConstant == ExpressionOperator::SumAdd )
            stream << "Add(" << arguments << ")";
        if constexpr( OperationParameterConstant == ExpressionOperator::SumSubtract )
            stream << "Subtract(" << arguments << ")";
        return stream.str();
    }
};

struct FactorStem : public MakeSuper< FactorStem, ExpressionNode >
{
    FactorType literal;
    using ThisType = FactorStem;
    constexpr FactorStem( const FactorType literal ) : literal( literal ) {}
    constexpr FactorStem( ThisType const& other ) = default;
    constexpr FactorStem( ThisType&& other ) = default;
    virtual std::string to_string() const override
    {
        std::stringstream stream;
        std::visit( [ & ]( auto data ){ 
                stream << "{" << data << "}"; 
            }, literal.factor );
        return stream.str();
    }
};

constexpr char natural_number_regex[] = "[0-9][0-9]*";
constexpr ctpg::regex_term< natural_number_regex > natural_number_term( "NaturalNumber" );


using FactorVariantType = std::variant< 
        ExpressionNode< ExpressionOperator::FactorMultiply >, 
        ExpressionNode< ExpressionOperator::FactorDivide >, 
        FactorStem 
    >;

using SumVariantType = std::variant< 
        ExpressionNode< ExpressionOperator::SumAdd >, 
        ExpressionNode< ExpressionOperator::SumSubtract >
    >;

constexpr ctpg::nterm< FactorVariantType > factor( "Factor" );
constexpr ctpg::nterm< SumVariantType > sum( "Sum" );
// constexpr ctpg::nterm< BaseNodeType& > parenthesis_scope( "ParenthesisScope" );

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
                sum
                //parenthesis_scope 
            ), 
        ctpg::rules( 
                factor( natural_number_term ) 
                        >= []( auto token ) {
                                return FactorType{ to_size_t( token ) };
                            }, 
                factor( factor, multiply_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token )
                            {
                                return std::visit( [ & ]( auto data ) {
                                        return data.multiply( *( new FactorStem{ to_size_t( next_token ) } ) );
                                    }, current_factor );
                            }, 
                factor( factor, divide_term, natural_number_term ) 
                        >= []( auto current_factor, auto, const auto& next_token ) {
                                return std::visit( [ & ]( auto data ) {
                                        return data.divide( *( new FactorStem{ to_size_t( next_token ) } ) );
                                    }, current_factor );
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
