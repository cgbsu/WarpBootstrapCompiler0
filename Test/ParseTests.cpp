#include <Warp/Testing/ParseTest.hpp>

namespace Warp::Parser::Testing
{
    int parse_test_main( int argc, char** args, size_t first_user_argument_index )
    {
        const auto& parser = ParserType::parser;
        for( size_t test_suite_index = 0; 
                test_suite_index < test_suite_names.size(); 
                ++test_suite_index )
        {
            if( std::string_view{ args[ first_user_argument_index ] } == test_suite_names[ test_suite_index ] )
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
}
