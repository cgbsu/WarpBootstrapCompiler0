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

int main( int argc, char** args )
{
    const auto& parser = Warp::Parser::DefaultParserType::parser;
    auto test_module = Warp::Parser::parse( parser, test_simple_switch_code );
    auto& test_alternative = *test_module.functions[ 0 ]->alternatives[ 0 ].alternatives[ 0 ];
    std::vector< Warp::CompilerRuntime::ValueType > arguments{ Warp::CompilerRuntime::ValueType{ static_cast< size_t >( 10 ) } };
    std::cout << "Mapping: the arguments of " << test_alternative.name << " to the values \n";
    for( size_t ii = 0; ii < arguments.size(); ++ii )
    {
        std::cout << "\t[ " << ii << " ]: " 
                << std::get< size_t >( arguments[ ii ] ) 
                << "\n";
    }
    auto mapping = Warp::CompilerRuntime::map_call_frame( test_alternative, arguments );
    if( mapping.has_value() == true )
    {
        auto mapping_result = mapping.value();
        std::cout << "Results: \n";
        for( auto& mapped_pair : mapping_result )
        {
            std::cout << "\t" << mapped_pair.first 
                    << " : " 
                    << std::get< size_t >( mapped_pair.second ) 
                    << "\n";
        }
        std::cout << "Testing constraints: \n";
        for( auto& constraint : test_alternative.input_constraints )
        {

            std::cout << "\tSatisfies Constraint? For Parameter: ";
            std::cout << constraint.name << ": ";
            std::cout << Warp::CompilerRuntime::satisfies_constraint( constraint.constraints, mapping_result ) 
                    << "\n";
        }
        std::cout << "Constraint Tests Complete\n";
    }
    else
    {
        std::cout << "Failed to map arguments! Unequal amounts of arguments! " 
                << arguments.size() << " arguments vs "
                << test_alternative.input_constraints.size() 
                << " parameters.\n";
    }
    std::cout << "Done :)\n";
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
