#include <Warp/Parser.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_PARSE_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSE_HPP

namespace Warp::Parser
{
    std::string_view break_code( std::string_view source_code, const char deliminator ) {
        return source_code.substr( 0, source_code.find_first_of( deliminator ) + 1 );
    }
    Warp::CompilerRuntime::Module parse( 
            const auto& parser, 
            std::string_view module_source_code, 
            const char deliminator = Warp::Utilities::to_char( FunctionOperators::FunctionDefintionComplete ) 
        )
    {
        Warp::CompilerRuntime::Module new_module;
        const size_t code_length = module_source_code.size();
        for( size_t ii = 0; ii < code_length; )
        {
            auto code = break_code( module_source_code.substr( ii ), deliminator );
            ii += code.size();
            if( const auto result = parser.parse( 
                        // ctpg::parse_options{}.set_verbose(), 
                        ctpg::buffers::string_buffer( code.data() ), std::cerr 
                    ); result.has_value() == true 
                )
                std::cout << "Hooray!\n";
            else
                std::cout << "Parsing failed!\n";
            // auto function_alternative = parser.parse( code );
        }

    }
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSE_HPP
