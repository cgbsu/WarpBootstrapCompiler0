// #include <Warp/ExpressionAnalysis.hpp>
#include <Warp/Parse.hpp>
#include <Warp/SimpleExecutor.hpp>
#include <Warp/Testing/ParseTest.hpp>

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

std::string test_recursive_code{ 
        "let raise( x : x > 1 ) :: x * raise( x - 1 );"
        "let raise( x : x = 1 ) :: 1 * 1;" // Bug to fix in the parse, cant just return 1. //
    };

std::string test_simple_switch_code{ 
        "let switch_function( x : x < 42 ) :: x * x;"
        "let switch_function( x : x <= 42 ) :: x + x;" 
    };

std::string test_multi_parameter_simple_switch_code{ 
        "let switch_function( x : x < 42, y : y < 300 ) :: x * x + y;"
        "let switch_function( x : x >= 42, y : y >= 300 ) :: x + x * y;" 
    };

int main( int argc, char** args )
{
    using Val = Warp::CompilerRuntime::ValueType;
    const auto& parser = Warp::Parser::DefaultParserType::parser;
    auto test_module = Warp::Parser::parse( parser, test_multi_parameter_simple_switch_code );
    auto& test_function = *test_module.functions[ 0 ];
    std::vector< Warp::CompilerRuntime::ValueType > arguments{ Val{ static_cast< size_t >( 10 ) }, Val{ static_cast< size_t >( 35 ) } };
    for( auto& alternatives : test_function.alternatives )
    {
        std::cout << "For alternatives with " << alternatives.number_of_parameters << " parameters: \n";
        size_t alternative_index = 0;
        for( auto* alternative : alternatives.alternatives ) {
            std::cout << "\tSatisfies Alternative [ " << alternative_index++ << " ]: "
                    << Warp::CompilerRuntime::satisfies_alternative_constraints( *alternative, arguments ) << "\n";
        }
    }
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
