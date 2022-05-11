#include <Warp/Parser.hpp>
#include <Warp/ExpressionAnalysis.hpp>

using ParserType = typename Warp::Parser::WarpParser< Warp::Parser::DefaultTypes >;

struct WarpTest {
    bool expected_result;
    std::string code;
};

template< typename ParserParameterType = ParserType >
bool test_parse( const WarpTest& test, const ParserParameterType& parser, bool display_result = true )
{
    const bool result = parser.parse( 
            // ctpg::parse_options{}.set_verbose(), 
            ctpg::buffers::string_buffer( test.code.c_str() ), std::cerr 
        ).has_value() == test.expected_result;
    if( display_result == true )
        std::cout << ( ( result == true ) ? "Pass: ": "Fail: " ) << test.code << "\n";
    return result;
}



#ifdef WARP_REPL

int main( int argc, char** args )
{
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

static std::array test_suite_names = {
        "alternative_calls"
    };

using TestSuiteType = std::vector< WarpTest >;

static TestSuiteType function_alternative_calls{
        WarpTest{ true, "let test( a : a < 64 ) :: test( 1 );" }, 
        WarpTest{ true, "let test( a : a < 64 ) :: test( a );" }, 
        WarpTest{ true, "let test( a : a < 64 ) :: 20 * test( a );" }, 
        WarpTest{ true, "let test( a : a < 64 ) :: test( a ) * 20;" }, 
        WarpTest{ true, "let test( a : a < 64 ) :: test( a ) + 10;" }, 
        WarpTest{ true, "let test( a : a < 64 ) :: test( a * a );" }, 
        WarpTest{ true, "let test( a : a < 64 ) :: test( a(), b( q, 4, 5646, 345345 * 445656 + 34 ), rdfg * 34534 );" }, 
        WarpTest{ true, "let test( a : a < 64 ) :: test( a, q, 4, 5646, 345345 * 445656 + 34, rdfg * 34534 );" }, 
        WarpTest{ true, "let test( a : a < 64 ) :: test( ttt(), q, 4, 5646, 345345 * 445656 + 34, rdfg * 34534 );" } 
    };

static std::vector< TestSuiteType > test_suits{ 
        function_alternative_calls 
    };

int main( int argc, char** args )
{
    const auto& parser = ParserType::parser;
    for( size_t test_suite_index = 0; 
            test_suite_index < test_suite_names.size(); 
            ++test_suite_index )
    {
        if( std::string_view{ args[ 1 ] } == test_suite_names[ test_suite_index ] )
        {
            const size_t number_of_tests = test_suits[ test_suite_index ].size();
            size_t number_of_tests_succeeded = 0;
            for( size_t ii = 0; 
                    ii < number_of_tests; 
                    ( test_parse( 
                            test_suits[ test_suite_index ][ ii++ ], 
                            parser, 
                            true 
                        ) == true ) 
                    ? ++number_of_tests_succeeded 
                    : number_of_tests_succeeded
                );
            if( number_of_tests_succeeded == number_of_tests )
                std::cout << "All tests succeeded!\n";
            else
            {
                std::cout << "("
                        << ( number_of_tests - number_of_tests_succeeded ) 
                        << "/" 
                        << number_of_tests 
                        << ") tests failed.\n";
            }
            break;
        }
    }


    return 0;
}

#endif // WARP_REPL
