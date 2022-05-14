#include <Warp/Function.hpp>
#include <optional>
#include <sstream>
#include <string>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_SIMPLE_EXECUTOR_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_SIMPLE_EXECUTOR_HPP

namespace Warp::CompilerRuntime 
{
    #define WARP_BOOTSTRAP_COMPILER_HEADER_SIMPLE_EXECUTOR_HPP_DEBUG true

    void debug_print( std::string message )  {
        if( WARP_BOOTSTRAP_COMPILER_HEADER_SIMPLE_EXECUTOR_HPP_DEBUG == true )
            std::cout << message;
    }

    using OptionalValueType = std::optional< AbstractSyntaxTree::ValueType >;
    using CallFrameType = std::unordered_map< std::string, AbstractSyntaxTree::ValueType >;

    template< typename ParameterConstant >
    concept BiNodeConcept = Warp::Utilities::IsAnyOfConcept< 
            Warp::Utilities::CleanType< ParameterConstant >, 
            Warp::Parser::ComparisonOperator, 
            Warp::Parser::BooleanOperator, 
            Warp::Parser::ExpressionOperator 
        >;

    template< auto NodeTypeParameterConstant >
    struct ExtractNodeType {
        constexpr static auto node_type = NodeTypeParameterConstant;
        ExtractNodeType( const Warp::AbstractSyntaxTree::Node< NodeTypeParameterConstant >& ) {}
    };

    template< template< typename, auto > typename FeedbackParameterType, typename ReturnParameterType >
    ReturnParameterType abstract_syntax_tree_callback( 
            const Warp::AbstractSyntaxTree::NodeVariantType& variant, 
            const CallFrameType& argument_values, 
            const std::vector< std::string >& experssion_log, 
            std::optional< Module >& module, 
            auto... additional_arguments 
        )
    {
        const auto& node = Warp::Utilities::to_const_reference( variant );
        return Warp::Utilities::visit< []( 
                    auto raw_node_pointer, 
                    const CallFrameType& argument_stack, 
                    std::vector< std::string >& log, 
                    std::optional< Module >& module, 
                    auto... arguments 
                ) 
            { 
               const decltype( *raw_node_pointer )& reference = *raw_node_pointer;
            //    std::cout << "Callback::NodeType: " << ( char ) decltype( ExtractNodeType( reference ) )::node_type << "\n";
               return FeedbackParameterType< 
                        ReturnParameterType, 
                        decltype( ExtractNodeType( reference ) )::node_type 
                    >::compute_value_of_expression( reference, argument_stack, log, module, arguments... ); 
            } >( node, argument_values, experssion_log, module, additional_arguments... );
    }

    template< typename, auto >
    struct Executor{};

    template< typename ReturnParameterType >
    struct Executor< ReturnParameterType, Warp::AbstractSyntaxTree::NodeType::Literal >
    {
        static ReturnParameterType compute_value_of_expression( 
                    const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Literal >& node, 
                    const CallFrameType& call_frame, 
                    std::vector< std::string >& experssion_log, 
                    std::optional< Module >& module, 
                    auto... additional_arguments 
                ) {
            auto value = Warp::Utilities::to_std_variant( node.value );
            std::visit( [ & ]( auto to_print ){ 
                    debug_print( std::string{ "literal(" } );
                    debug_print( std::to_string( to_print ) );
                    debug_print( std::string{ ")" } );
                }, value );
            return value;
        }
    };

    template< typename ReturnParameterType >
    struct Executor< ReturnParameterType, Warp::AbstractSyntaxTree::NodeType::BooleanLiteral >
    {
        static ReturnParameterType compute_value_of_expression( 
                    const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::BooleanLiteral >& node, 
                    const CallFrameType& call_frame, 
                    std::vector< std::string >& experssion_log, 
                    std::optional< Module >& module, 
                    auto... additional_arguments 
                ) {
            debug_print( std::string{ "bool(" } );
            debug_print( std::to_string( node.value ) );
            debug_print( std::string{ ")" } );
            return AbstractSyntaxTree::ValueType{ node.value };
        }
    };

    template< typename ReturnParameterType >
    struct Executor< ReturnParameterType, Warp::AbstractSyntaxTree::NodeType::Identifier >
    {
        static ReturnParameterType compute_value_of_expression( 
                    const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Identifier >& node, 
                    const CallFrameType& call_frame, 
                    std::vector< std::string >& experssion_log, 
                    std::optional< Module >& module, 
                    auto... additional_arguments 
                ) {

            AbstractSyntaxTree::ValueType value = call_frame.at( node.string );
            std::visit( [ & ]( auto to_print ){ 
                    debug_print( std::string{ "id:" } );
                    debug_print( std::to_string( to_print ) );
                }, value );
            return value;
        }
    };

    template< typename ReturnParameterType, BiNodeConcept auto OperatorParameterConstant >
    struct Executor< ReturnParameterType, OperatorParameterConstant >
    {
        static ReturnParameterType compute_value_of_expression( 
                const Warp::AbstractSyntaxTree::Node< OperatorParameterConstant >& node, 
                const CallFrameType& call_frame, 
                std::vector< std::string >& experssion_log, 
                std::optional< Module >& module, 
                auto... additional_arguments 
            )
        {
            const auto value_from = [ & ]( const Warp::AbstractSyntaxTree::NodeVariantType& from ) -> ReturnParameterType 
                    { 
                        return abstract_syntax_tree_callback< Executor, ReturnParameterType >( 
                                from, 
                                call_frame, 
                                experssion_log, 
                                module, 
                                additional_arguments... 
                            ); 
                    };
            debug_print( std::string{ " {" } );
            using OperationNodeType = typename Warp::Utilities::CleanType< decltype( node ) >;
            const auto left = value_from( node.left );
            debug_print( std::string{ " " } );
            debug_print( std::to_string( static_cast< char >( OperationNodeType::operation ) ) );
            debug_print( std::string{ " " } );
            const auto right = value_from( node.right );
            auto result = Utilities::variant_operation< [ & ]( auto left, auto right ) { return OperationNodeType::operate( left, right ); } >( 
                    left, 
                    right 
                );
            debug_print( std::string{ "}:[" } );
            debug_print( std::to_string( std::get< bool >( result.value() ) ) );
            debug_print( std::string{ "] " } );
            return result;
        }
    };

    template< typename OutputParameterType >
    struct Executor< OutputParameterType, Warp::Parser::BooleanOperator::LogicalNot >
    {
        static OutputParameterType compute_value_of_expression( 
                const Warp::AbstractSyntaxTree::Node< Warp::Parser::BooleanOperator::LogicalNot >& node, 
                const CallFrameType& call_frame, 
                std::vector< std::string >& expression_log, 
                std::optional< Module >& module, 
                auto... additional_arguments 
            )
        {
            const auto value_from = [ & ]( const Warp::AbstractSyntaxTree::NodeVariantType& from ) { 
                    return abstract_syntax_tree_callback< Executor, OutputParameterType >( from, call_frame, expression_log, module, additional_arguments... ); 
                };
            using OperationNodeType = Warp::Utilities::CleanType< decltype( node ) >;
            debug_print( std::string{ " {!" } );
            auto operation = value_from( node.child );
            debug_print( std::string{ "}:[" } );
            auto result = Utilities::variant_operation< 
                    [ & ]( auto operhand ) { return OperationNodeType::operate( operhand ); } >( 
                        operation
                );
            debug_print( std::string{ "] " } );
            return std::optional{ result };
        }
    };

    template< typename OutputParameterType >
    struct Executor< OutputParameterType, Warp::Parser::FunctionOperators::FunctionResult >
    {
        static OutputParameterType compute_value_of_expression( 
                const Warp::AbstractSyntaxTree::Node< Warp::Parser::FunctionOperators::FunctionResult >& node, 
                    const CallFrameType& call_frame, 
                    std::vector< std::string >& experssion_log, 
                    std::optional< Module >& module, 
                    auto... additional_arguments 
            )
        {
            // std::cout << "A\n";
            auto result = call_frame.at( Warp::Parser::function_result_as_string );
            std::visit( [ & ]( auto to_print ){ 
                    debug_print( Warp::Parser::function_result_as_string );
                    debug_print( std::string{ ":" } );
                    debug_print( std::to_string( to_print ) );
                }, result );
            return result;
            // std::cerr << "Error::NotYetImplemented: Executor::compute_value_of_expression for FunctionResult! Returning 0\n";
            // return 0;
        }
    };

    template< typename OutputParameterType >
    struct Executor< OutputParameterType, Warp::AbstractSyntaxTree::NodeType::FunctionCall >
    {
        static OutputParameterType compute_value_of_expression( 
                const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::FunctionCall >& node, 
                const CallFrameType& call_frame, 
                std::vector< std::string >& expression_log, 
                std::optional< Module >& module, 
                auto... additional_arguments 
            )
        {
            std::cerr << "Error::NotYetImplemented: Executor::compute_value_of_expression for FunctionCall! Returning 0\n";
            return 0;
        }
    };


    template< typename OutputParameterType >
    struct Executor< OutputParameterType, Warp::AbstractSyntaxTree::NodeType::Unconstrained >
    {
        static OutputParameterType compute_value_of_expression( 
                const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Unconstrained >& node, 
                const CallFrameType& call_frame, 
                std::vector< std::string >& expression_log, 
                std::optional< Module >& module, 
                auto... additional_arguments 
            )
        {
            // std::cout << "B\n";
            auto result = call_frame.at( Warp::Parser::function_result_as_string );
            std::visit( [ & ]( auto to_print ){ 
                    debug_print( Warp::Parser::function_result_as_string );
                    debug_print( std::string{ "::Unconstrainted (Returning True):" } );
                    debug_print( std::to_string( to_print ) );
                }, result );
            return OptionalValueType{ true };
            // return result;
            // std::cerr << "Error::NotYetImplemented: Executor::compute_value_of_expression for Unconstrained! Returning true\n";
            // return true;
        }
    };

    enum class ConstraintApplication {
        Input, 
        Output 
    };

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
            std::optional< AbstractSyntaxTree::ValueType > result = std::nullopt 
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

    template< typename ValueParameterType >
    AbstractSyntaxTree::ValueType make_value( ValueParameterType value ) {
        return AbstractSyntaxTree::ValueType{ Warp::Utilities::make_literal( value ) };
    }

    template<>
    AbstractSyntaxTree::ValueType make_value( bool value ) {
        return AbstractSyntaxTree::ValueType{ value };
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
        if( result.has_value() != true )
        {
            debug_print( std::string{ "satisfies_constraint(const NodeVariantType&, const CallFrameType&) : "
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
            debug_print( std::string{ "satisfies_constraint(const NodeVariantType&, const CallFrameType&) : "
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
            debug_print( std::to_string( satisfied ) );
            const auto& call_frame_result = call_frame.value();
            for( auto& parameter : alternative.input_constraints ) {
                debug_print( std::string{ " && " } );
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

    template< typename ReturnParameterType = AbstractSyntaxTree::ValueType >
    std::optional< ReturnParameterType > evaluate_expression( 
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
        if( result.has_value() != true ) {
            debug_print( std::string{ "satisfies_constraint(const NodeVariantType&, const CallFrameType&) : "
                    "bool::Error: evaluating constraint! No value processed.\n" 
                } );
        }
        return result;
    }

    template< typename QueryParameterType >
    auto is_of_type( auto canidate )
    {
        static_assert( 
                std::is_same< std::decay_t< QueryParameterType >, std::decay_t< decltype( canidate ) > >::value, 
                "Error:: Attempt to parse literal in expression with a type that differed than the parsed literal type" 
            );
        return canidate;
    }

}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_SIMPLE_EXECUTOR_HPP
