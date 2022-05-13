#include <Warp/Parse.hpp>

namespace Warp::CompilerRuntime
{
    const Warp::CompilerRuntime::FunctionAlternative& add_new_alternative_to_function( 
            Warp::CompilerRuntime::Function& function, 
            const Warp::CompilerRuntime::FunctionAlternative& new_alternative 
        )
    {
        const auto number_of_parameters = new_alternative.input_constraints.size();
        // Easy access when you know the number of arguments in constant time, alternatives[ number_of_arguments ] //
        const size_t original_size = function.alternatives.size();
        if( original_size < number_of_parameters )
        {
            function.alternatives.resize( number_of_parameters + 1 );
            for( size_t ii = original_size; 
                    ii < number_of_parameters + 1; 
                    function.alternatives[ ii++ ].number_of_parameters = ii 
                );
        }
        return *function.alternatives[ number_of_parameters - 1 ].alternatives.emplace_back( 
                new Warp::CompilerRuntime::FunctionAlternative{ 
                        new_alternative.identifier, 
                        new_alternative.expression, 
                        new_alternative.return_constraint, 
                        new_alternative.input_constraints, 
                        new_alternative.name 
                    }
            );
    }

    const FunctionWithAltenativeProxy add_new_function_to_module_from_alternative( 
            Warp::CompilerRuntime::Module& to_append, 
            const Warp::CompilerRuntime::FunctionAlternative& new_alternative 
        )
    {
        Warp::CompilerRuntime::Function& new_function = *to_append.functions.emplace_back( 
                new Warp::CompilerRuntime::Function{ 
                        new_alternative.identifier, 
                        new_alternative.name//, 
                        // std::vector< FunctionAlternatives >{} 
                    } 
            );
        return FunctionWithAltenativeProxy{ new_function, add_new_alternative_to_function( new_function, new_alternative ) };
    }

    const FunctionWithAltenativeProxy add_alternative_to_module( 
            Warp::CompilerRuntime::Module& to_append, 
            const Warp::CompilerRuntime::FunctionAlternative& new_alternative, 
            bool add_function_from_alternative_if_function_does_not_exist
        )
    {
        bool found_function = false;
        for( Function* function : to_append.functions )
        {
            if( function->name == new_alternative.name )
            {
                return FunctionWithAltenativeProxy{ 
                        *function, 
                        add_new_alternative_to_function( *function, new_alternative ) 
                    };
            }
        }
        if( add_function_from_alternative_if_function_does_not_exist == false )
        {
            std::string error = "add_alternative_to_module( Module&, FunctionAlternative& ):FunctionWithAltenativeProxy"
                    "::Error: Attempt to add function alternative to module, when function does not exist in module, and AddIfFunctionDoesNotExistParameterConstant"
                    "is set to false.";
            std::cout << error << "\n";
            throw std::runtime_error{ error };
        }
        else
            return add_new_function_to_module_from_alternative( to_append, new_alternative );
    }
}

namespace Warp::Parser
{
    std::string break_code( std::string source_code, const char deliminator ) {
        return source_code.substr( 0, source_code.find_first_of( deliminator ) + 1 );
    }
}
