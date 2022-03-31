#include <Warp/Expression.hpp>

using NaturalNumberConstantExpression = ConstantExpression< size_t >;

constexpr static auto factor_parser = ctpg::parser{ 
        NaturalNumberConstantExpression::factor, 
        NaturalNumberConstantExpression::terms, 
        NaturalNumberConstantExpression::nterms, 
        NaturalNumberConstantExpression::rules
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
