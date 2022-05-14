#include <Warp/SimpleExecutor.hpp>

namespace Warp::CompilerRuntime
{
    Function* function_from_module( const Module& to_search, std::string name )
    {
        for( Function* function : to_search.functions ) {
            if( function->name == name )
                return function;
        }
        return nullptr;
    }

    CallFrameType map_call_frame_no_check( 
            const Warp::CompilerRuntime::FunctionAlternative& alternative, 
            std::vector< AbstractSyntaxTree::ValueType > values 
        ) noexcept
    {
        const size_t amount_of_parameters = alternative.input_constraints.size();
        CallFrameType new_call_frame;
        for( size_t ii = 0; ii < amount_of_parameters; ++ii )
            new_call_frame.insert( std::pair< std::string, AbstractSyntaxTree::ValueType >( alternative.input_constraints[ ii ].name, values[ ii ] ) );
        return new_call_frame;
    }


    CallFrameType map_call_frame_no_check( 
            const Warp::CompilerRuntime::FunctionAlternative& alternative, 
            std::vector< AbstractSyntaxTree::ValueType > values, 
            AbstractSyntaxTree::ValueType result 
        ) noexcept
    {
        CallFrameType new_call_frame = map_call_frame_no_check( alternative, values );
        new_call_frame.insert( std::pair< std::string, AbstractSyntaxTree::ValueType >( Warp::Parser::function_result_as_string, result ) );
        return new_call_frame;
    }

    std::optional< CallFrameType > map_call_frame( 
            const Warp::CompilerRuntime::FunctionAlternative& alternative, 
            const std::vector< AbstractSyntaxTree::ValueType >& values, 
            std::optional< AbstractSyntaxTree::ValueType > result  
        )
    {
        if( values.size() == alternative.input_constraints.size() )
        {
            if( result.has_value() == true )
                return std::optional{ map_call_frame_no_check( alternative, values, result.value() ) };
            else
                return std::optional{ map_call_frame_no_check( alternative, values ) };
        }
        else
            return std::nullopt;
    }

    bool satisfies_constraint( 
            const Warp::AbstractSyntaxTree::NodeVariantType& constraint, 
            const CallFrameType& argument_values, 
            std::vector< std::string >& log, 
            std::optional< Module >& module 
        ) 
    {
        auto result = abstract_syntax_tree_callback< Executor, OptionalValueType >( 
                constraint, 
                argument_values, 
                log, 
                module
            );
        if( result.has_value() == false )
        {
            debug_print( log, std::string{ "satisfies_constraint(const NodeVariantType&, const CallFrameType&) : "
                    "bool::Error: evaluating constraint! No value processed.\n" 
                } );
            return false;
        }
        if( auto constraint_evaluation = std::get_if< bool >( &result.value() ); constraint_evaluation != nullptr )
            return *constraint_evaluation; // Carefull C++ apparently allows implicit conversion from pointer to bool. //
        else if( auto conversion_result = std::visit( 
                            []( auto data ) -> std::optional< AbstractSyntaxTree::ValueType > 
                            {
                                using DataType = std::remove_pointer_t< decltype( data ) >;
                                if( data == static_cast< DataType >( 1 ) || data == static_cast< DataType >( 0 ) )
                                    return std::optional{ AbstractSyntaxTree::ValueType{ static_cast< bool >( data ) } };
                                else
                                    return std::nullopt;
                            }, 
                            result.value() );
                    conversion_result.has_value() == true 
                )
            return std::get< bool >( conversion_result.value() );
        else
        {
            debug_print( log, std::string{ "satisfies_constraint(const NodeVariantType&, const CallFrameType&) : "
                    "bool::Error: evaluating constraint! Non-boolean-convertable value returned.\n" 
                } );
            return false;
        }
        return false;
    }

    bool satisfies_alternative_constraints( 
            const Warp::CompilerRuntime::FunctionAlternative& alternative, 
            const std::vector< AbstractSyntaxTree::ValueType >& values, 
            std::vector< std::string >& log, 
            std::optional< Module >& module  
        )
    {
        auto call_frame = map_call_frame( alternative, values );
        if( bool satisfied = call_frame.has_value(); satisfied == true )
        {
            debug_print( log, std::to_string( satisfied ) );
            const auto& call_frame_result = call_frame.value();
            for( auto& parameter : alternative.input_constraints ) {
                debug_print( log, std::string{ " && " } );
                satisfied = satisfied && satisfies_constraint( parameter.constraints, call_frame_result, log, module );
            }
            return satisfied;
        }
        return false;
    }

    bool satisfies_alternative_constraints( 
            const Warp::CompilerRuntime::FunctionAlternative& alternative, 
            const std::vector< AbstractSyntaxTree::ValueType >& values, 
            AbstractSyntaxTree::ValueType result, 
            std::vector< std::string >& log, 
            std::optional< Module >& module  
        )
    {
        auto call_frame = map_call_frame( alternative, values, result );
        if( bool satisfied = call_frame.has_value(); satisfied == true ) {
            const auto& call_frame_result = call_frame.value();
            return satisfied && satisfies_constraint( alternative.return_constraint, call_frame_result, log, module );
        }
        return false;
    }

    template<>
    AbstractSyntaxTree::ValueType make_value( bool value ) {
        return AbstractSyntaxTree::ValueType{ value };
    }

}
