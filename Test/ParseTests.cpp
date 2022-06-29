/*#include <Warp/Testing/ParseTest.hpp>

namespace Warp::Parser::Testing
{
    int parse_test_main( int argc, char** args, size_t first_user_argument_index )
    {
        if( argc < ( first_user_argument_index + 1 ) ) {
            std::cout << "Parser Testing::Sorry, I dont know what to do, enter \"help\" to see a list of availible test data sets.\n";
            return 0;
        }
        else if( std::string_view{ args[ first_user_argument_index ] } == help_flag )
        {
            std::cout << "\nParser Testing::Usage::Run a Test Suite: " 
                    << ( ( first_user_argument_index != 0 ) ? args[ 0 ] : "tester" ) 
                    << " DATA_SET_NAME_HERE\n";
            std::cout << "\nParser Testing::Availible Test-Sets: \n";
            for( size_t test_suite_index = 0; 
                    test_suite_index < test_suite_names.size(); 
                    ++test_suite_index )
                std::cout << "* : " << test_suite_names[ test_suite_index ] << "\n";
            std::cout << "\nThanks for reading :)\n\n";
        }
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
*/

