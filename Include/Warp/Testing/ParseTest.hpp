// On purpose. //
#include <Warp/Parser.hpp>
#include <Warp/Parse.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_PARSE_TEST_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSE_TEST_HPP

namespace Warp::Parser::Testing
{

    constexpr static const char* help_flag = "help";

    using ParserType = Warp::Parser::DefaultParserType;

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

    using TestSuiteType = std::vector< WarpTest >;

    //////////////////////////////////////////////////////////////////////////////////////////////
    // Making them extern variables I can split `parse_test_main` and the data it operates on ////
    // into two seperate translation units, so when I update the test data, all the headers may //
    // not need to be recompiled. ////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////

    // TODO: ADD BOOLEAN LOGICAL TESTS!!!!!!

    extern std::array< const char*, 4 > test_suite_names;
    extern TestSuiteType function_alternative_constrained_return_parameter;
    extern TestSuiteType function_alternative_calls;
    extern TestSuiteType basic_function_alternatives;
    extern TestSuiteType factor_calls;
    extern std::vector< TestSuiteType > test_suits;
    extern std::string bulk_parse_test;

    int parse_test_main( int argc, char** args, size_t first_user_argument_index = 1 );

    Warp::CompilerRuntime::Module&& test_bulk_parse( 
            const auto& parser, 
            std::string source_code, 
            ctpg::parse_options parse_options = ctpg::parse_options{}, //{}.set_verbose(), 
            char deliminator = Warp::Utilities::to_char( FunctionOperators::FunctionDefintionComplete ) 
        )
    {
        // return Warp::Parser::Testing::parse_test_main( argc, args, 1 );
        auto module = Warp::Parser::parse( parser, source_code, deliminator, parse_options );
        for( auto function : module.functions )
        {
            std::cout << "For function: " << function->name << "\n";
            for( auto& alternative : function->alternatives )
            {
                std::cout << "\t" << alternative.alternatives.size() 
                        << " alternatives with " 
                        << alternative.number_of_parameters 
                        << " paramters\n";
            }
        }
        return module;
    }
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSE_TEST_HPP
