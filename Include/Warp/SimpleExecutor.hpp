#include <Warp/Function.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_SIMPLE_EXECUTOR_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_SIMPLE_EXECUTOR_HPP

namespace Warp::CompilerRuntime 
{
    template< auto NodeTypeParameterConstant >
    struct ExtractNodeType {
        constexpr static auto node_type = NodeTypeParameterConstant;
        ExtractNodeType( const Warp::AbstractSyntaxTree::Node< NodeTypeParameterConstant >& ) {}
    };

    template< template< typename, typename, auto > typename FeedbackParameterType, typename ReturnParameterType, typename InputParameterType >
    ReturnParameterType abstract_syntax_tree_callback( const Warp::AbstractSyntaxTree::NodeVariantType& variant, auto... additional_arguments )
    {
        const auto& node = Warp::Utilities::to_const_reference( variant );
        return Warp::Utilities::visit< []( auto raw_node_pointer, auto... arguments ) { 
               const decltype( *raw_node_pointer )& reference = *raw_node_pointer;
               return FeedbackParameterType< 
                        ReturnParameterType, 
                        InputParameterType, 
                        decltype( ExtractNodeType( reference ) )::node_type 
                    >::compute_value_of_expression( reference, arguments... ); 
            } >( node, additional_arguments... );
    }

    template< typename, typename, auto >
    struct Executor{};

    template< typename ReturnParameterType, typename InputParameterType >
    struct Executor< ReturnParameterType, InputParameterType, Warp::AbstractSyntaxTree::NodeType::Literal >
    {
        static ReturnParameterType compute_value_of_expression( 
                const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Literal >& node, 
                auto... additional_arguments 
            )
        {
            auto value = Warp::Utilities::visit< []( auto value_pointer, auto... ) -> InputParameterType {
                    return *value_pointer;
                } >( Warp::Utilities::to_const_reference( node.value.factor ), additional_arguments... );
            return value;
        }
    };

    template< typename ReturnParameterType, typename InputParameterType >
    struct Executor< ReturnParameterType, InputParameterType, Warp::AbstractSyntaxTree::NodeType::BooleanLiteral >
    {
        static ReturnParameterType compute_value_of_expression( 
                    const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::BooleanLiteral >& node, 
                    auto... additional_arguments 
                ) {
            return node.value;
        }
    };

    template< typename ReturnParameterType, typename InputParameterType >
    struct Executor< ReturnParameterType, InputParameterType, Warp::AbstractSyntaxTree::NodeType::Identifier >
    {
        static ReturnParameterType compute_value_of_expression( 
                    const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Identifier >& node, 
                    auto... additional_arguments 
                ) {
            std::cerr << "Error::NotYetImplemented: Executor::compute_value_of_expression for Identifiers! Returning 0\n";
            return 0;
        }
    };

    template< typename ReturnParameterType, typename InputParameterType, Warp::Parser::ExpressionOperator OperatorParameterConstant >
    struct Executor< ReturnParameterType, InputParameterType, OperatorParameterConstant >
    {
        static ReturnParameterType compute_value_of_expression( 
                const Warp::AbstractSyntaxTree::Node< OperatorParameterConstant >& node, 
                auto... additional_arguments 
            )
        {
            const auto value_from = []( const Warp::AbstractSyntaxTree::NodeVariantType& from, auto... arguments ) -> InputParameterType { 
                    return abstract_syntax_tree_callback< Executor, ReturnParameterType, InputParameterType >( from, arguments... ); 
                };
            using OperationNodeType = std::decay_t< Warp::Utilities::CleanType< decltype( node ) > >;
            return OperationNodeType::operate( 
                    value_from( node.left, additional_arguments... ), 
                    value_from( node.right, additional_arguments... ) 
                );
        }
    };

    template<>
    struct Executor< bool, bool, Warp::Parser::BooleanOperator::LogicalNot >
    {
        static bool compute_value_of_expression( 
                const Warp::AbstractSyntaxTree::Node< Warp::Parser::BooleanOperator::LogicalNot >& node, 
                auto... additional_arguments 
            )
        {
            const auto value_from = []( const Warp::AbstractSyntaxTree::NodeVariantType& from, auto... arguments ) -> bool { 
                    return abstract_syntax_tree_callback< Executor, bool, bool >( from, arguments... ); 
                };
            using OperationNodeType = std::decay_t< Warp::Utilities::CleanType< decltype( node ) > >;
            return OperationNodeType::operate( 
                    value_from( node.child, additional_arguments... ) 
                );
        }
    };


    template< Warp::Parser::BooleanOperator OperatorParameterConstant >
    struct Executor< bool, bool, OperatorParameterConstant >
    {
        static bool compute_value_of_expression( 
                const Warp::AbstractSyntaxTree::Node< OperatorParameterConstant >& node, 
                auto... additional_arguments 
            )
        {
            const auto value_from = []( const Warp::AbstractSyntaxTree::NodeVariantType& from, auto... arguments ) -> bool { 
                    return abstract_syntax_tree_callback< Executor, bool, bool >( from, arguments... ); 
                };
            using OperationNodeType = std::decay_t< Warp::Utilities::CleanType< decltype( node ) > >;
            return OperationNodeType::operate( 
                    value_from( node.left, additional_arguments... ), 
                    value_from( node.right, additional_arguments... ) 
                );
        }
    };

    template< typename InputParameterType, Warp::Parser::ComparisonOperator OperatorParameterConstant >
    struct Executor< bool, InputParameterType, OperatorParameterConstant >
    {
        static bool compute_value_of_expression( 
                const Warp::AbstractSyntaxTree::Node< OperatorParameterConstant >& node, 
                auto... additional_arguments 
            )
        {
            const auto value_from = []( const Warp::AbstractSyntaxTree::NodeVariantType& from, auto... arguments ) -> InputParameterType { 
                    return abstract_syntax_tree_callback< Executor, bool, InputParameterType >( from, arguments... ); 
                };
            using OperationNodeType = std::decay_t< Warp::Utilities::CleanType< decltype( node ) > >;
            return OperationNodeType::operate( 
                    value_from( node.left, additional_arguments... ), 
                    value_from( node.right, additional_arguments ... ) 
                );
        }
    };

    using ValueType = std::variant< Warp::AbstractSyntaxTree::LiteralType, bool >;
    using CallFrameType = std::unordered_map< std::string, ValueType >;

    CallFrameType map_call_frame_no_check( 
            const Warp::CompilerRuntime::FunctionAlternative& alternative, 
            std::vector< ValueType > values 
        ) noexcept
    {
        const size_t amount_of_parameters = alternative.input_constraints.size();
        CallFrameType new_call_frame;
        for( size_t ii = 0; ii < amount_of_parameters; ++ii )
            new_call_frame.insert( { alternative.input_constraints[ ii ].name, values[ ii ] } );
        return new_call_frame;
    }

    std::optional< CallFrameType > map_call_frame( 
            const Warp::CompilerRuntime::FunctionAlternative& alternative, 
            const std::vector< ValueType >& values 
        )
    {
        if( values.size() == alternative.input_constraints.size() )
            return std::optional{ map_call_frame_no_check( alternative, values ) };
        else
            return std::nullopt;
    }

    template< typename ValueParameterType >
    ValueType make_value( ValueParameterType value ) {
        return ValueType{ Warp::Utilities::make_literal( value ) };
    }

    template<>
    ValueType make_value( bool value ) {
        return ValueType{ value };
    }

    // enum class ConstraintApplication {
    //     Input, 
    //     Output 
    // };

    // bool satisfies_constraint( const Warp::AbstractSyntaxTree::NodeVariantType& constraint, const CallFrameType& call_frame )
    // {

    // }

    // template< ConstraintApplication ConstraintTypeParameterConstant = Input >
    // bool evaluate_if_fits_constraints( const Warp::CompilerRuntime::FunctionAlternative& alternative, const Warp::CompilerRuntime::CallType& call )
    // {
    //     for( auto& parameter : alternative.input_constraints )
    // }

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
