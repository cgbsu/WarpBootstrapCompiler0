// #include <Warp/ExpressionAnalysis.hpp>
#include <Warp/Parse.hpp>
#include <Warp/SimpleExecutor.hpp>
#include <Warp/Testing/ParseTest.hpp>
#include <optional>

// #define WARP_REPL

#ifdef WARP_REPL

int main( int argc, char** args )
{
    const auto& parser = Warp::Parser::DefaultParserType::parser;
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
            std::cout << "Have result!\n";
            auto result = parse_result.value();
            // Warp::Analysis::abstract_syntax_tree_callback< Warp::Analysis::Executor, size_t >( parse_result.value() );
            // std::cout << "\nResults: \n";
            // std::cout << "Function Name Hash: " << result.identifier << "\n";
            // for( auto& parameter : result.input_constraints )
            //     std::cout << "\tParameter Name Hash: " << parameter.identifier << "\n";

        }
        else
            std::cerr << "Error failed to parse!\n";
        std::cout << "Enter prompt please: ";
    }
    return 0;
}

#else // WARP_REPL

std::string test{ 
        "let switch_function( x : x < 42, y : y < 300 ) @ < 200 :: x * x + y;"
        "let switch_function( x : x >= 42, y : y >= 300 ) @ = 10 :: x + x * y;" 
    };

int main( int argc, char** args )
{
    using Val = Warp::AbstractSyntaxTree::ValueType;
    const auto& parser = Warp::Parser::DefaultParserType::parser;
    auto test_module = Warp::Parser::parse( parser, test );
    std::vector< Warp::AbstractSyntaxTree::ValueType > arguments{ Val{ static_cast< size_t >( 10 ) }, Val{ static_cast< size_t >( 35 ) } };
    std::vector< std::string > log;
    auto module = std::optional{ test_module };
    auto result = Warp::CompilerRuntime::call_function_with_values( 
            arguments, 
            *test_module.functions[ 0 ], 
            module, 
            log 
        );
    auto expression_result_raw = std::visit( []( auto data ) { 
                std::cout << "\tResult: " << data << " (returning as size_t)\n";
                return static_cast< size_t >( data );
            }, 
            result.value() 
        );
return 0;
}


// int main( int argc, char** args )
// {
//     const auto& parser = Warp::Parser::DefaultParserType::parser;
//     std::string code = Warp::Parser::Testing::bulk_parse_test;
//     Warp::Parser::Testing::test_bulk_parse( parser, code );
//     return 0;
// }

#endif // WARP_REPL
