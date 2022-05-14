#include <Warp/Function.hpp>

namespace Warp::CompilerRuntime
{
    std::optional< size_t > find_function_index_in_module( const Module& module, std::string function_name )
    {
        const size_t number_of_functions = module.functions.size();
        for( size_t ii = 0; ii < number_of_functions; ++ii )
        {
            if( module.functions[ ii ]->name == function_name )
                return ii;
        }
        return std::nullopt;
    }

    std::optional< LogEntry > log_call( 
            Module& module, 
            const FunctionAlternative& caller, 
            std::vector< AbstractSyntaxTree::ValueType >& inputs, 
            const AbstractSyntaxTree::ValueType result 
        )
    {
        const size_t number_of_parameters = inputs.size();
        if( module.forward_input_log.size() <= number_of_parameters )
            module.forward_input_log.resize( number_of_parameters + 1 );
        const auto function_search_result = find_function_index_in_module( module, caller.name );
        if( function_search_result.has_value() == false )
            return std::nullopt;
        const size_t function_index = function_search_result.value();
        auto alternatives = module.functions[ function_index ]->alternatives[ number_of_parameters ].alternatives;
        const FunctionAlternative* caller_pointer = std::addressof( caller );
        if( alternatives.size() <= 0 )
            return std::nullopt;
        const size_t amount_of_alternatives = alternatives.size();
        size_t alternative_index = 0; 
        for( ; alternative_index < amount_of_alternatives; ++alternative_index ) {
            if( caller_pointer == alternatives[ alternative_index ] )
                break;
        }
        auto new_call_log_entry = LogEntry{ 
                function_index, 
                alternative_index, 
                number_of_parameters, 
                inputs, 
                result 
            };
        module.forward_input_log[ number_of_parameters ].push_back( new_call_log_entry );
        return new_call_log_entry;
    }
}
