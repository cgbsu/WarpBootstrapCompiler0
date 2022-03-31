#include <Warp/Expression.hpp>

using NaturalNumberConstantExpression = ConstantExpression< 
        to_size_t, 
        size_t 
    >;


/*constexpr static auto plus_term = ctpg::char_term{ 
        '+', 
        1, 
        ctpg::associativity::ltor 
    };
*/
constexpr static auto factor_parser = ctpg::parser{ 
        NaturalNumberConstantExpression::factor, 
        // ctpg::terms(plus_term), 
        NaturalNumberConstantExpression::terms, 
        // ctpg::nterms(NaturalNumberConstantExpression::factor), 
        NaturalNumberConstantExpression::nterms, 
        // ctpg::rules()// 
        NaturalNumberConstantExpression::rules
        // ctpg::rules( 
        //     NaturalNumberConstantExpression::factor(NaturalNumberConstantExpression::plus_term ) >= [](auto){ return 1; }, 
        //     NaturalNumberConstantExpression::factor(NaturalNumberConstantExpression::literal_term, NaturalNumberConstantExpression::plus_term ) >= [](auto ,auto){ return 2; }
        //  )
    };

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
            std::cout << "Result: " << parse_result.value() << "\n";
        else
            std::cerr << "Error failed to parse!\n";
        std::cout << "Enter prompt please: ";
    }
    return 0;
}
