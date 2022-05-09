#include <Warp/AbstractSyntaxTreeUtilities.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_ANYLSIS_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_ANYLSIS_HPP

namespace Warp::Analysis
{
    template< auto NodeTypeParameterConstant >
    struct ExtractNodeType {
        constexpr static auto node_type = NodeTypeParameterConstant;
        ExtractNodeType( const Warp::AbstractSyntaxTree::Node< NodeTypeParameterConstant >& ) {}
    };

    template< template< typename, auto > typename FeedbackParameterType, typename ReturnParameterType >
    ReturnParameterType abstract_syntax_tree_callback( const Warp::AbstractSyntaxTree::NodeVariantType& variant )
    {
        const auto& node = Warp::Utilities::to_const_reference( variant );
        return Warp::Utilities::visit< []( auto raw_node_pointer ) { 
               const decltype( *raw_node_pointer )& reference = *raw_node_pointer;
               return FeedbackParameterType< 
                    ReturnParameterType, 
                    decltype( ExtractNodeType( reference ) )::node_type >::compute_value_of_expression( reference ); 
            } >( node );
    }

    template< typename, auto >
    struct Executor{};


    template< typename ExpressionParameterType >
    struct Executor< ExpressionParameterType, Warp::AbstractSyntaxTree::NodeType::Literal >
    {
        static ExpressionParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Literal >& node )
        {
            auto value = Warp::Utilities::visit< []( auto value_pointer ) -> ExpressionParameterType {
                    return *value_pointer;
                } >( Warp::Utilities::to_const_reference( node.value.factor ) );
            return value;
        }
    };

    template< typename ExpressionParameterType >
    struct Executor< ExpressionParameterType, Warp::AbstractSyntaxTree::NodeType::BooleanLiteral >
    {
        static ExpressionParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::BooleanLiteral >& node ) {
            return node.value;
        }
    };

    template< typename ExpressionParameterType >
    struct Executor< ExpressionParameterType, Warp::AbstractSyntaxTree::NodeType::Identifier >
    {
        static ExpressionParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Identifier >& node ) {
            std::cout << "Error::NotYetImplemented: Executor::compute_value_of_expression for Identifiers! Returning 0\n";
            return 0;
        }
    };

    template< typename ExpressionParameterType, Warp::Parser::ExpressionOperator OperatorParameterConstant >
    struct Executor< ExpressionParameterType, OperatorParameterConstant >
    {
        static ExpressionParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< OperatorParameterConstant >& node )
        {
            const auto value_from = []( const Warp::AbstractSyntaxTree::NodeVariantType& from ) -> ExpressionParameterType { 
                    return abstract_syntax_tree_callback< Executor, ExpressionParameterType >( from ); 
                };
            using OperationNodeType = std::decay_t< Warp::Utilities::CleanType< decltype( node ) > >;
            return OperationNodeType::operate( 
                    value_from( node.left ), 
                    value_from( node.right ) 
                );
        }
    };

    template< typename ExpressionParameterType >
    struct Executor< ExpressionParameterType, Warp::Parser::BooleanOperator::LogicalNot >
    {
        static ExpressionParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< Warp::Parser::BooleanOperator::LogicalNot >& node )
        {
            const auto value_from = []( const Warp::AbstractSyntaxTree::NodeVariantType& from ) -> ExpressionParameterType { 
                    return abstract_syntax_tree_callback< Executor, ExpressionParameterType >( from ); 
                };
            using OperationNodeType = std::decay_t< Warp::Utilities::CleanType< decltype( node ) > >;
            return OperationNodeType::operate( 
                    value_from( node.child ) 
                );
        }
    };


    template< typename ExpressionParameterType, Warp::Parser::BooleanOperator OperatorParameterConstant >
    struct Executor< ExpressionParameterType, OperatorParameterConstant >
    {
        static ExpressionParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< OperatorParameterConstant >& node )
        {
            const auto value_from = []( const Warp::AbstractSyntaxTree::NodeVariantType& from ) -> ExpressionParameterType { 
                    return abstract_syntax_tree_callback< Executor, ExpressionParameterType >( from ); 
                };
            using OperationNodeType = std::decay_t< Warp::Utilities::CleanType< decltype( node ) > >;
            return OperationNodeType::operate( 
                    value_from( node.left ), 
                    value_from( node.right ) 
                );
        }
    };

    template< typename ExpressionParameterType, Warp::Parser::ComparisonOperator OperatorParameterConstant >
    struct Executor< ExpressionParameterType, OperatorParameterConstant >
    {
        static ExpressionParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< OperatorParameterConstant >& node )
        {
            const auto value_from = []( const Warp::AbstractSyntaxTree::NodeVariantType& from ) -> ExpressionParameterType { 
                    return abstract_syntax_tree_callback< Executor, ExpressionParameterType >( from ); 
                };
            using OperationNodeType = std::decay_t< Warp::Utilities::CleanType< decltype( node ) > >;
            return OperationNodeType::operate( 
                    value_from( node.left ), 
                    value_from( node.right ) 
                );
        }
    };



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

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_HPP
