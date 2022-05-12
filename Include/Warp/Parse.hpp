#include <Warp/Parser.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_PARSE_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSE_HPP

namespace Warp::CompilerRuntime
{
    struct FunctionWithAltenativeProxy {
        const Warp::CompilerRuntime::Function& function;
        const Warp::CompilerRuntime::FunctionAlternative& alternative;
    };

    const Warp::CompilerRuntime::FunctionAlternative& add_new_alternative_to_function( 
            Warp::CompilerRuntime::Function& function, 
            const Warp::CompilerRuntime::FunctionAlternative& new_alternative 
        );
    
    const FunctionWithAltenativeProxy add_new_function_to_module_from_alternative( 
            Warp::CompilerRuntime::Module& to_append, 
            const Warp::CompilerRuntime::FunctionAlternative& new_alternative 
        );

    const FunctionWithAltenativeProxy add_alternative_to_module( 
            Warp::CompilerRuntime::Module& to_append, 
            const Warp::CompilerRuntime::FunctionAlternative& new_alternative, 
            bool add_function_from_alternative_if_function_does_not_exist = false 
        );
}

namespace Warp::Parser
{
    std::string_view break_code( std::string_view source_code, const char deliminator );

    Warp::CompilerRuntime::Module parse( 
            const auto& parser, 
            std::string_view module_source_code, 
            const char deliminator = Warp::Utilities::to_char( FunctionOperators::FunctionDefintionComplete ), 
            ctpg::parse_options parser_options = ctpg::parse_options{} 
        )
    {
        Warp::CompilerRuntime::Module new_module;
        const size_t code_length = module_source_code.size();
        for( size_t ii = 0; ii < code_length; )
        {
            auto code = break_code( module_source_code.substr( ii ), deliminator );
            ii += code.size();
            if( const auto result = parser.parse( 
                            parser_options, // ctpg::parse_options{}.set_verbose(), 
                            ctpg::buffers::string_buffer( code.data() ), std::cerr 
                        ); result.has_value() == true 
                    ) {
                const auto new_alternative = result.value();
                add_alternative_to_module( new_module, new_alternative, true );
            }
            else
                std::cerr << "Parsing failed for: \n" << code << "\n";
            // auto function_alternative = parser.parse( code );
        }
        return new_module;
    }
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSE_HPP
