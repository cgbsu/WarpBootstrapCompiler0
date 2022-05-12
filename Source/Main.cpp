// #include <Warp/Testing/ParseTest.hpp>
// #include <Warp/ExpressionAnalysis.hpp>
#include <Warp/Parse.hpp>
//
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


std::string test { 
    "let test( a : a < 64 ) @ < 1 :: test( 12 );"
    "let test( a : a < 64 ) @ <= a :: test( a );"
    // "let test( a : a < 64 ) @ * @ < a + 1 :: 20 * test( a );"
    // "let test( a : a < 64 ) @ + @ < a * 34 :: 20 + test( a );"
    // "let test( a : a < 64, b : test( b ) < 34 ) @ + @ < a * a :: test( a ) * 20;"
    // "let test( a : a < 64 ) 1 < @ :: test( a ) + 10;"
    // "let test( a : a < 64, b : test( b ) < 34, c : c > 100 * a ) a * a > @ :: test( a * a );"
    // "let test( a : a < 64 ) @ <= a && @ < 1 :: test( a(), b( q, 4, 5646, 345345 * 445656 + 34 ), rdfg * 34534 );"
    // "let test( a : a < 64, c : c > 100 * a ) @ + @ < a * 34 <-> @ <= a && @ < 1 :: test( a, q, 4, 5646, 345345 * 445656 + 34, rdfg * 34534 );"
    // "let test( a : a < 64, b : test( b ) < 34 ) @ + @ < a * 34 <-> @ < a + b :: test( ttt(), q, 4, 5646, 345345 * 445656 + 34, rdfg * 34534 );"
    // "let test( a : a < 64 ) @ + @ < a * 34 || @ < a + b :: test( ttt(), q, 4 * another_test(), 5646, 345345 * 445656 + 34, rdfg * 34534 );"
    // "let test( a : a < 64 ) a + b * a * 34 = @ + @  || @ < a + b :: test( ttt(), q, 4 * another_test(), 5646, 345345 * another_test( abc, 123 ) + 34, rdfg * 34534 );"
    // "let test( a : a < 64, b : test( b ) < 34 ) @ + a >= a + b && a + b * a * 34 = @ + @ || @ < a + b :: test( ttt(), q, 4 * another_test(), 5646, 345345 + another_test( abc, 123 ) * 34, rdfg * 34534 );"
    // "let test( a : a < 64, b : test( b ) < 34, c : c > 100 * a, d : d <= c + a * 100 ) a * 42 = @ :: test( ttt(), q, 4 / another_test(), 5646, 345345 / another_test( abc, 123 ) - 34, rdfg / 34534 );"
    // "let test( a : a < 64, c : c > 100 * a ) @ < 10 && a > @ :: test( ttt(), q, 4 / another_test(), 5646, 345345 - another_test( abc, 123 ) - 34, rdfg / 34534 );" 
};

int main( int argc, char** args )
{
    const auto& parser = Warp::Parser::DefaultParserType::parser;
    // return Warp::Parser::Testing::parse_test_main( argc, args, 1 );
    Warp::Parser::parse( parser, test );//, ';', ctpg::parse_options{}.set_verbose() );
    return 0;
}

#endif // WARP_REPL
