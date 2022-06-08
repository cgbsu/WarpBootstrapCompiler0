#include <Warp/Parse.hpp>
#include <Warp/SimpleExecutor.hpp>
#include <Warp/Testing/ParseTest.hpp>

std::string log_cat( std::vector< std::string >& log );

int main( int argc, char** args )
{
    std::string module_source_code = "";
    if( argc > 1 )
    {
        std::string module_path{ args[ 1 ] };
        std::cout << "Note: Attempting to load " << module_path << "... ";
        if( auto read_result = Warp::Utilities::read_file( module_path ); read_result.has_value() ) {
            module_source_code = read_result.value();
            std::cout << "Success!\n";
        }
        else
            std::cout << "Failure!\n";
    }
    std::string exit = "thanks :)";
    std::cout << "Thank me to exit \"" << exit << "\"\n";
    using Val = Warp::AbstractSyntaxTree::ValueType;
    const auto& parser = Warp::Parser::DefaultParserType::parser;
    const auto& call_parser = Warp::Parser::WarpParser< 
            Warp::Parser::DefaultTypes, 
            Warp::Parser::NonTerminalTerms::Factor 
        >::parser;
    std::vector< std::string > log;
    auto main_module = Warp::Parser::parse( parser, module_source_code );
    for( std::string input; input != "thanks :)"; )
    {
        std::cout << "--> ";
        std::getline( std::cin, input );
        std::stringstream call_error_stream, parse_error_stream;
        if( auto call_parse = call_parser.parse( ctpg::buffers::string_buffer( input.c_str() ), call_error_stream ); 
                call_parse.has_value() == true )
        {
            auto call_pointer = call_parse.value();
            auto place_holder_call_frame = Warp::CompilerRuntime::CallFrameType{};
            auto module = std::optional{ main_module };
            if( auto result = Warp::CompilerRuntime::evaluate_expression( 
                        call_pointer, 
                        place_holder_call_frame, 
                        log, 
                        module 
                    ); result.has_value() == true )
                std::visit( []( auto value ) { std::cout << "\nGot: " << value << "\n"; }, result.value() );
            else
                std::cout << "Failed to evaluate expression :(\n";
        }
        else if( auto parse_result = parser.parse( ctpg::buffers::string_buffer( input.c_str() ), parse_error_stream ); 
                parse_result.has_value() == true )
        {
            auto parse_pointer = parse_result.value();
            auto alternative = Warp::CompilerRuntime::add_alternative_to_module( 
                    main_module, 
                    parse_pointer, 
                    true 
                );
            std::cout << "Added alternative to module.\n";
        }
        else if( input == exit ) {
            std::cout << "Have a nice day :)\n";
            break;
        }
        else
        {
            std::cout << "Sorry, I didn't understand that.\n"
                    << "Not a call because: " << call_error_stream.str() << "\n"
                    << "Not a function definition because " << parse_error_stream.str() << "\n";
        }

    }
    return 0;
}

std::string log_cat( std::vector< std::string >& log )
{
    std::stringstream log_buffer;
    for( std::string entry : log )
        log_buffer << entry;
    return log_buffer.str();
}
