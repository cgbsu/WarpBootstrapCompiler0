#include <Warp/Function.hpp>
#include <optional>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_SIMPLE_EXECUTOR_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_SIMPLE_EXECUTOR_HPP

namespace Warp::CompilerRuntime 
{
    #ifndef WARP_BOOTSTRAP_COMPILER_HEADER_SIMPLE_EXECUTOR_HPP_DEBUG
        #define WARP_BOOTSTRAP_COMPILER_HEADER_SIMPLE_EXECUTOR_HPP_DEBUG false
    #endif

    void debug_print( std::vector< std::string >& log, std::string message );
    
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

    Function* function_from_module( const Module& to_search, std::string name );

    CallFrameType map_call_frame_no_check( 
            const Warp::CompilerRuntime::FunctionAlternative& alternative, 
            std::vector< AbstractSyntaxTree::ValueType > values 
        ) noexcept;
    CallFrameType map_call_frame_no_check( 
            const Warp::CompilerRuntime::FunctionAlternative& alternative, 
            std::vector< AbstractSyntaxTree::ValueType > values, 
            AbstractSyntaxTree::ValueType result 
        ) noexcept;
    std::optional< CallFrameType > map_call_frame( 
            const Warp::CompilerRuntime::FunctionAlternative& alternative, 
            const std::vector< AbstractSyntaxTree::ValueType >& values, 
            std::optional< AbstractSyntaxTree::ValueType > result = std::nullopt 
        );
    bool satisfies_constraint( 
            const Warp::AbstractSyntaxTree::NodeVariantType& constraint, 
            const CallFrameType& argument_values, 
            std::vector< std::string >& log, 
            std::optional< Module >& module 
        );
    bool satisfies_alternative_constraints( 
            const Warp::CompilerRuntime::FunctionAlternative& alternative, 
            const std::vector< AbstractSyntaxTree::ValueType >& values, 
            std::vector< std::string >& log, 
            std::optional< Module >& module  
        );
    bool satisfies_alternative_constraints( 
            const Warp::CompilerRuntime::FunctionAlternative& alternative, 
            const std::vector< AbstractSyntaxTree::ValueType >& values, 
            AbstractSyntaxTree::ValueType result, 
            std::vector< std::string >& log, 
            std::optional< Module >& module  
        );

    template< typename ValueParameterType >
    AbstractSyntaxTree::ValueType make_value( ValueParameterType value ) {
        return AbstractSyntaxTree::ValueType{ Warp::Utilities::make_literal( value ) };
    }

    template<>
    AbstractSyntaxTree::ValueType make_value( bool value );

    template< typename ReturnParameterType = AbstractSyntaxTree::ValueType >
    std::optional< ReturnParameterType > evaluate_expression( 
            const Warp::AbstractSyntaxTree::NodeVariantType& expression, 
            const CallFrameType& argument_values, 
            std::vector< std::string >& log, 
            std::optional< Module >& module 
        );
    template< typename ReturnParameterType = AbstractSyntaxTree::ValueType >
    std::optional< std::vector< FunctionAlternative* > > select_alternative_based_on_inputs(
            std::vector< AbstractSyntaxTree::ValueType >& values, 
            Function& function, 
            std::optional< Module >& module, 
            std::vector< std::string >& log 
        );

    template< typename ReturnParameterType = AbstractSyntaxTree::ValueType >
    std::optional< std::vector< std::pair< ReturnParameterType, FunctionAlternative* > > > select_alternative_based_on_outputs(
            std::vector< AbstractSyntaxTree::ValueType >& input_values, 
            std::vector< FunctionAlternative* > canidates, 
            std::optional< Module >& module, 
            std::vector< std::string >& log 
        );

    template< typename ReturnParameterType = AbstractSyntaxTree::ValueType >
    std::optional< std::pair< ReturnParameterType, std::optional< LogEntry > > > call_function_with_values(
            std::vector< AbstractSyntaxTree::ValueType >& values, 
            Function& function, 
            std::optional< Module >& module, 
            std::vector< std::string >& log 
        );

    template< template< typename, auto > typename FeedbackParameterType, typename ReturnParameterType >
    ReturnParameterType abstract_syntax_tree_callback( 
            const Warp::AbstractSyntaxTree::NodeVariantType& variant, 
            const CallFrameType& argument_values, 
            const std::vector< std::string >& log, 
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
            } >( node, argument_values, log, module, additional_arguments... );
    }

    template< typename, auto >
    struct Executor{};

    template< typename ReturnParameterType >
    struct Executor< ReturnParameterType, Warp::AbstractSyntaxTree::NodeType::Literal >
    {
        static ReturnParameterType compute_value_of_expression( 
                    const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Literal >& node, 
                    const CallFrameType& call_frame, 
                    std::vector< std::string >& log, 
                    std::optional< Module >& module, 
                    auto... additional_arguments 
                ) {
            auto value = Warp::Utilities::to_std_variant( node.value );
            std::visit( [ & ]( auto to_print ){ 
                    debug_print( log, std::string{ "literal(" } );
                    debug_print( log, std::to_string( to_print ) );
                    debug_print( log, std::string{ ")" } );
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
                    std::vector< std::string >& log, 
                    std::optional< Module >& module, 
                    auto... additional_arguments 
                ) {
            debug_print( log, std::string{ "bool(" } );
            debug_print( log, std::to_string( node.value ) );
            debug_print( log, std::string{ ")" } );
            return AbstractSyntaxTree::ValueType{ node.value };
        }
    };

    template< typename ReturnParameterType >
    struct Executor< ReturnParameterType, Warp::AbstractSyntaxTree::NodeType::Identifier >
    {
        static ReturnParameterType compute_value_of_expression( 
                    const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Identifier >& node, 
                    const CallFrameType& call_frame, 
                    std::vector< std::string >& log, 
                    std::optional< Module >& module, 
                    auto... additional_arguments 
                ) {

            AbstractSyntaxTree::ValueType value = call_frame.at( node.string );
            std::visit( [ & ]( auto to_print ){ 
                    debug_print( log, node.string );
                    debug_print( log, ":" );
                    debug_print( log, std::to_string( to_print ) );
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
                std::vector< std::string >& log, 
                std::optional< Module >& module, 
                auto... additional_arguments 
            )
        {
            const auto value_from = [ & ]( const Warp::AbstractSyntaxTree::NodeVariantType& from ) -> ReturnParameterType 
                    { 
                        return abstract_syntax_tree_callback< Executor, ReturnParameterType >( 
                                from, 
                                call_frame, 
                                log, 
                                module, 
                                additional_arguments... 
                            ); 
                    };
            debug_print( log, std::string{ " {" } );
            using OperationNodeType = typename Warp::Utilities::CleanType< decltype( node ) >;
            const auto left = value_from( node.left );
            std::stringstream middle;
            middle << " " << static_cast< char >( OperationNodeType::operation ) << " ";
            debug_print( log, middle.str() );
            const auto right = value_from( node.right );
            auto result = Utilities::variant_operation< [ & ]( auto left, auto right ) { return OperationNodeType::operate( left, right ); } >( 
                    left, 
                    right 
                );
            debug_print( log, std::string{ "}:[" } );
            if( result.has_value() == true )
                debug_print( log, Utilities::to_std_string( result.value() ) );
            else
                debug_print( log, std::string{ "no-value" } );
            debug_print( log, std::string{ "] " } );
            return result;
        }
    };

    template< typename OutputParameterType >
    struct Executor< OutputParameterType, Warp::Parser::BooleanOperator::LogicalNot >
    {
        static OutputParameterType compute_value_of_expression( 
                const Warp::AbstractSyntaxTree::Node< Warp::Parser::BooleanOperator::LogicalNot >& node, 
                const CallFrameType& call_frame, 
                std::vector< std::string >& log, 
                std::optional< Module >& module, 
                auto... additional_arguments 
            )
        {
            const auto value_from = [ & ]( const Warp::AbstractSyntaxTree::NodeVariantType& from ) { 
                    return abstract_syntax_tree_callback< Executor, OutputParameterType >( from, call_frame, log, module, additional_arguments... ); 
                };
            using OperationNodeType = Warp::Utilities::CleanType< decltype( node ) >;
            debug_print( log, std::string{ " {!" } );
            auto operation = value_from( node.child );
            debug_print( log, std::string{ "}:[" } );
            auto result = Utilities::variant_operation< 
                    [ & ]( auto operhand ) { return OperationNodeType::operate( operhand ); } >( 
                        operation
                );
            debug_print( log, std::string{ "] " } );
            return std::optional{ result };
        }
    };

    template< typename OutputParameterType >
    struct Executor< OutputParameterType, Warp::Parser::FunctionOperators::FunctionResult >
    {
        static OutputParameterType compute_value_of_expression( 
                const Warp::AbstractSyntaxTree::Node< Warp::Parser::FunctionOperators::FunctionResult >& node, 
                    const CallFrameType& call_frame, 
                    std::vector< std::string >& log, 
                    std::optional< Module >& module, 
                    auto... additional_arguments 
            )
        {
            auto result = call_frame.at( Warp::Parser::function_result_as_string );
            std::visit( [ & ]( auto to_print ){ 
                    debug_print( log, Warp::Parser::function_result_as_string );
                    debug_print( log, std::string{ ":" } );
                    debug_print( log, std::to_string( to_print ) );
                }, result );
            return result;
        }
    };

    template< typename OutputParameterType >
    struct Executor< OutputParameterType, Warp::AbstractSyntaxTree::NodeType::FunctionCall >
    {
        static OutputParameterType compute_value_of_expression( 
                const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::FunctionCall >& node, 
                const CallFrameType& call_frame, 
                std::vector< std::string >& log, 
                std::optional< Module >& module, 
                auto... additional_arguments 
            )
        {
            using OutputType = typename Utilities::UnwrapOptional< OutputParameterType >::Type;
            const auto value_from = [ & ]( const Warp::AbstractSyntaxTree::NodeVariantType& from ) { 
                    return abstract_syntax_tree_callback< Executor, OutputParameterType >( from, call_frame, log, module, additional_arguments... ); 
                };
            const size_t number_of_parameters = node.arguments.size();
            std::vector< AbstractSyntaxTree::ValueType > values;
            for( size_t ii = 0; ii < number_of_parameters; ++ii )
            {
                auto argument = evaluate_expression< OutputType >( node.arguments[ ii ], call_frame, log, module );
                if( argument.has_value() == false ) {
                    debug_print( log, std::string{ "FunctionCall::Error: Argument failed to be evaluated.\n" } );
                    return std::nullopt;
                }
                values.push_back( argument.value() );
            }
            if( module.has_value() == false ) {
                debug_print( log, std::string{ "FunctionCall::Error: Evaluation does not have a module!\n" } );
                return std::nullopt;
            }
            auto function = function_from_module( module.value(), node.function_name );
            if( function == nullptr ) {
                debug_print( log, std::string{ "FunctionCall::Error: Function " } + node.function_name + std::string{ " not found in module\n" } );
                return std::nullopt;
            }
            Function& non_const_function = *function;
            if( auto function_result = call_function_with_values< OutputType >(
                        values, 
                        non_const_function, 
                        module, 
                        log 
                    ); function_result.has_value() == true )
                return function_result.value().first;
            else {
                debug_print( log, std::string{ "FunctionCall::Error: Function " } + node.function_name + std::string{ " failed to return a value!\n" } );
                return std::nullopt;
            }
        }
    };


    template< typename OutputParameterType >
    struct Executor< OutputParameterType, Warp::AbstractSyntaxTree::NodeType::Unconstrained >
    {
        static OutputParameterType compute_value_of_expression( 
                const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Unconstrained >& node, 
                const CallFrameType& call_frame, 
                std::vector< std::string >& log, 
                std::optional< Module >& module, 
                auto... additional_arguments 
            )
        {
            auto result = call_frame.at( Warp::Parser::function_result_as_string );
            std::visit( [ & ]( auto to_print ){ 
                    debug_print( log, Warp::Parser::function_result_as_string );
                    debug_print( log, std::string{ "::Unconstrainted (Returning True):" } );
                    debug_print( log, std::to_string( to_print ) );
                }, result );
            return OptionalValueType{ true };
        }
    };

        template< typename ReturnParameterType = AbstractSyntaxTree::ValueType >
    std::optional< ReturnParameterType > evaluate_expression( 
            const Warp::AbstractSyntaxTree::NodeVariantType& expression, 
            const CallFrameType& argument_values, 
            std::vector< std::string >& log, 
            std::optional< Module >& module 
        )
    {
        auto result = abstract_syntax_tree_callback< Executor, OptionalValueType >( 
                expression, 
                argument_values, 
                log, 
                module 
            );
        if( result.has_value() != true )
        {
            debug_print( log, std::string{ "satisfies_constraint(const NodeVariantType&, const CallFrameType&) : "
                    "bool::Error: evaluating constraint! No value processed.\n" 
                } );
        }
        return result;
    }

    template< typename ReturnParameterType = AbstractSyntaxTree::ValueType >
    std::optional< std::vector< FunctionAlternative* > > select_alternative_based_on_inputs(
            std::vector< AbstractSyntaxTree::ValueType >& values, 
            Function& function, 
            std::optional< Module >& module, 
            std::vector< std::string >& log 
        )
    {
        if( const size_t number_of_parameters = values.size(); number_of_parameters < function.alternatives.size() )
        {
            std::vector< FunctionAlternative* > input_canidates;
            auto alternatives = function.alternatives[ number_of_parameters ];
            debug_print( log, std::string{ "New Number Of Parameters: " } + std::to_string( alternatives.number_of_parameters ) + std::string{ "\n" } );
            auto actual_alternatives = alternatives.alternatives;
            debug_print( log, std::string{ "Number of alternatives " } + std::to_string( actual_alternatives.size() ) + std::string{ "\n" } );
            if( actual_alternatives.size() <= 0 ) {
                debug_print( log, std::string{ "No alternatives for given number of arguments, returning nullopt.\n" } );
                return std::nullopt;
            }
            for( FunctionAlternative* alternative : actual_alternatives )
            {
                // auto mapping = map_call_frame( alternative, values );
                if( satisfies_alternative_constraints( *alternative, values, log, module ) == true ) {
                    debug_print( log, std::string{ "\nInput constraint satisfied\n" } );
                    input_canidates.push_back( alternative );
                }
                else
                    debug_print( log, std::string{ "\nInput NOT constraint satisfied\n" } );
            }
            debug_print( log, std::string{ "Number of canidates: " } + std::to_string( input_canidates.size() ) + std::string{ "\n" } );
            if( input_canidates.size() == 0 )
                return std::nullopt;
            return std::optional{ input_canidates };
        }
        return std::nullopt;
    }

    template< typename ReturnParameterType = AbstractSyntaxTree::ValueType >
    std::optional< std::vector< std::pair< ReturnParameterType, FunctionAlternative* > > > select_alternative_based_on_outputs(
            std::vector< AbstractSyntaxTree::ValueType >& input_values, 
            std::vector< FunctionAlternative* > canidates, 
            std::optional< Module >& module, 
            std::vector< std::string >& log 
        )
    {
        if( canidates.size() <= 0 )
        {
            debug_print( log, std::string{ 
                    "template<typename> select_alternative_based_on_outputs("
                    "std::vector< ValueType >&, std::vector< FunctionAlternative* >, "
                    "std::optional< Module >&, std::vector< std::string >& >:<typename>::"
                    "Error: no canidates.\n"
                } );
            return std::nullopt;
        }
        std::vector< std::pair< ReturnParameterType, FunctionAlternative* > > conforment_results;
        for( FunctionAlternative* alternative : canidates )
        {
            auto mapping = map_call_frame( *alternative, input_values );
            if( mapping.has_value() == false )
            {
                debug_print( log, std::string{ 
                        "template<typename> select_alternative_based_on_outputs("
                        "std::vector< ValueType >&, std::vector< FunctionAlternative* >, "
                        "std::optional< Module >&, std::vector< std::string >& >:<typename>::"
                        "Error: Mapping failed!\n"
                    } );
            }
            const AbstractSyntaxTree::NodeVariantType& expression = alternative->expression;
            auto result = evaluate_expression< ReturnParameterType >( expression, mapping.value(), log, module );
            if( result.has_value() == true )
            {
                const FunctionAlternative& alternative_ = *alternative;
                if( satisfies_alternative_constraints( alternative_, input_values, result.value(), log, module ) == true ) {
                    debug_print( log, std::string{ "\nOutput constraint satisfied\n" } );
                    conforment_results.push_back( std::pair{ result.value(), alternative } );
                }
            }
        }
        return conforment_results;
    }

    template< typename ReturnParameterType >
    std::optional< std::pair< ReturnParameterType, std::optional< LogEntry > > > call_function_with_values(
            std::vector< AbstractSyntaxTree::ValueType >& values, 
            Function& function, 
            std::optional< Module >& module, 
            std::vector< std::string >& log 
        )
    {
        auto input_selection_results = select_alternative_based_on_inputs( values, function, module, log );
        if( input_selection_results.has_value() == false )
        {
            debug_print( log, std::string{ 
                    "template< typename > call_function_with_values( "
                    "const std::vector< ValueType >&, const Function&, const Module&, "
                    "std::vector< std::string >& ):std::optional< typename >::"
                    "Error: No alternative for function that has "
                } + std::to_string( values.size() ) + std::string{ " parameters.\n" } );
            return std::nullopt;
        }
        auto input_canidates = input_selection_results.value();
        auto output_canidates_result = select_alternative_based_on_outputs( values, input_canidates, module, log );
        
        if( output_canidates_result.has_value() == false )
        {
            debug_print( log, std::string{ 
                    "FunctionCall::Error: No suitible alternative found for function call!\n"
                } );
            return std::nullopt;
        }
        if( output_canidates_result.value().size() == 1 )
        {
            auto final_result = output_canidates_result.value()[ 0 ];
            if( module.has_value() )
            {
                return std::optional{ std::pair{ 
                        final_result.first, 
                        log_call( module.value(), *final_result.second, values, final_result.first ) 
                    } };
            }
            else {
                return std::optional{ std::pair{ 
                        final_result.first, 
                        std::nullopt 
                    } };               
            }
        }
        debug_print( log, std::string{ 
                "FunctionCall::Error: Prototype-quality software, mutliple "
                "alternatives satisfy both input and output constraints.\n" 
            } );
        return std::nullopt;
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
