#include <iostream>
// #include <optional>
#include <string_view>
// #include <any>
// #include <variant>

#include <ctpg.hpp>


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
}

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
    Literal = 'L', 
    NoOperation = '\0'
};

struct Node
{
    const ExpressionOperator operation;
    const Node* left;
    const Node* right;
    const size_t* value;
    constexpr Node() 
            : left( nullptr ), operation( ExpressionOperator::NoOperation ), right( nullptr ), value( nullptr ) {}
    constexpr Node( 
            const Node* left, 
            const ExpressionOperator operation, 
            const Node* right ) 
        : left( left ), operation( operation ), right( right ), value( nullptr ) {}
    constexpr Node( const size_t* value ) 
            : left( nullptr ), operation( ExpressionOperator::NoOperation ), right( nullptr ), value( value ) {}
    constexpr Node( const Node& other ) = default;
    constexpr Node( Node&& other ) = default;
    constexpr Node& operator=( const Node& other ) = default;
    constexpr Node& operator=( Node&& other ) = default;
};

bool clean_up_node( const Node* node )
{
    bool result = true;
    if( node->left != nullptr ) {
        result = result && clean_up_node( node->left );
        delete node->left;
    }
    if( node->right != nullptr ) {
        result = result && clean_up_node( node->right );
        delete node->right;
    }
    if( node->value != nullptr )
        delete node->value;
    return result;
}

constexpr static ctpg::nterm< Node > factor{ "Factor" };
constexpr static ctpg::nterm< Node > sum{ "Sum" };
constexpr static ctpg::nterm< Node > parenthesis_scope{ "ParenthesisScope" };

constexpr static char natural_number_regex[] = "[0-9][0-9]*";
constexpr static ctpg::regex_term< natural_number_regex > natural_number_term{ "NaturalNumber" };

constexpr static ctpg::char_term plus_term( 
        static_cast< char >( ExpressionOperator::SumAdd ), 1, ctpg::associativity::ltor 
    );
constexpr static ctpg::char_term minus_term( 
        static_cast< char >( ExpressionOperator::SumSubtract ), 1, ctpg::associativity::ltor 
    );
constexpr static ctpg::char_term multiply_term( 
        static_cast< char >( ExpressionOperator::FactorMultiply ), 2, ctpg::associativity::ltor 
    );
constexpr static ctpg::char_term divide_term( 
        static_cast< char >( ExpressionOperator::FactorDivide ), 2, ctpg::associativity::ltor 
    );
constexpr static ctpg::char_term left_parenthesis_term( '(', 3, ctpg::associativity::ltor );
constexpr static ctpg::char_term right_parenthesis_term( ')', 3, ctpg::associativity::ltor );


constexpr ctpg::parser factor_parser( 
        factor, 
        ctpg::terms( 
                /*multiply_term, 
                divide_term, 
                plus_term, 
                minus_term, */
                natural_number_term/*, 
                left_parenthesis_term, 
                right_parenthesis_term */
            ), 
        ctpg::nterms( 
                factor/*, 
                sum, 
                parenthesis_scope */
            ), 
        ctpg::rules( 
                factor( natural_number_term ) 
                        >= []( auto token ) {
                                return Node{ new size_t( Utility::to_integral< size_t >( token ) ) };
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
                            ctpg::parse_options{}.set_verbose(), 
                            ctpg::buffers::string_buffer( input.c_str() ), std::cerr 
                    ); 
                    parse_result.has_value() == true 
                )
        {
            // char* result;
            // std::cout << "Result: " << parse_result.value() << "\n";
        }
        else
            std::cerr << "Error failed to parse!\n";
        std::cout << "Enter prompt please: ";
    }
    return 0;
}

