#include <Warp/Parser.hpp>
#include <Warp/ExpressionAnalysis.hpp>

int main( int argc, char** args )
{
    using ParserType = typename Warp::Parser::WarpParser< Warp::Parser::DefaultTypes >;
    const auto& parser = ParserType::parser;
    std::cout << "Hello world! Please enter your expression: ";
    for( std::string input = "2+2"/*"4*10+2"*/; input != "thanks :)"; std::getline( std::cin, input ) )
    {
        std::cout << "Got: " << input << "\n";
        if( auto parse_result = parser.parse( 
                            // ctpg::parse_options{}.set_verbose(), 
                            ctpg::buffers::string_buffer( input.c_str() ), std::cerr 
                    ); 
                    parse_result.has_value() == true 
                )
        {
            auto data = Warp::Analysis::abstract_syntax_tree_callback< Warp::Analysis::Computer, size_t >( parse_result.value() );
            std::cout << "\nResult: " 
                    << data 
                    << "\n\n";
        }
        else
            std::cerr << "Error failed to parse!\n";
        std::cout << "Enter prompt please: ";
    }
    return 0;
}
