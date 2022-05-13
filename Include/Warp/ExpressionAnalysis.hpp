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

    template< template< typename, typename auto > typename FeedbackParameterType, typename ReturnParameterType, typename InputParameterType >
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

    template< typename, typename auto >
    struct Executor{};

    template< typename ReturnParameterType, typename InputParameterType >
    struct Executor< ReturnParameterType, InputParameterType, Warp::AbstractSyntaxTree::NodeType::Literal >
    {
        static ReturnParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Literal >& node )
        {
            auto value = Warp::Utilities::visit< []( auto value_pointer ) -> InputParameterType {
                    return *value_pointer;
                } >( Warp::Utilities::to_const_reference( node.value.factor ) );
            return value;
        }
    };

    template< typename ReturnParameterType, typename InputParameterType >
    struct Executor< ReturnParameterType, InputParameterType, Warp::AbstractSyntaxTree::NodeType::BooleanLiteral >
    {
        static ReturnParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::BooleanLiteral >& node ) {
            return node.value;
        }
    };

    template< typename ReturnParameterType, typename InputParameterType >
    struct Executor< ReturnParameterType, InputParameterType, Warp::AbstractSyntaxTree::NodeType::Identifier >
    {
        static ReturnParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Identifier >& node ) {
            std::cout << "Error::NotYetImplemented: Executor::compute_value_of_expression for Identifiers! Returning 0\n";
            return 0;
        }
    };

    template< typename ReturnParameterType, typename InputParameterType, Warp::Parser::ExpressionOperator OperatorParameterConstant >
    struct Executor< ReturnParameterType, InputParameterType, OperatorParameterConstant >
    {
        static ReturnParameterType, InputParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< OperatorParameterConstant >& node )
        {
            const auto value_from = []( const Warp::AbstractSyntaxTree::NodeVariantType& from ) -> InputParameterType { 
                    return abstract_syntax_tree_callback< Executor, ReturnParameterType, InputParameterType >( from ); 
                };
            using OperationNodeType = std::decay_t< Warp::Utilities::CleanType< decltype( node ) > >;
            return OperationNodeType::operate( 
                    value_from( node.left ), 
                    value_from( node.right ) 
                );
        }
    };

    template<>
    struct Executor< bool, bool, Warp::Parser::BooleanOperator::LogicalNot >
    {
        static ReturnParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< Warp::Parser::BooleanOperator::LogicalNot >& node )
        {
            const auto value_from = []( const Warp::AbstractSyntaxTree::NodeVariantType& from ) -> InputParameterType { 
                    return abstract_syntax_tree_callback< Executor, bool, bool >( from ); 
                };
            using OperationNodeType = std::decay_t< Warp::Utilities::CleanType< decltype( node ) > >;
            return OperationNodeType::operate( 
                    value_from( node.child ) 
                );
        }
    };


    template< Warp::Parser::BooleanOperator OperatorParameterConstant >
    struct Executor< bool, bool, OperatorParameterConstant >
    {
        static ReturnParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< OperatorParameterConstant >& node )
        {
            const auto value_from = []( const Warp::AbstractSyntaxTree::NodeVariantType& from ) -> InputParameterType { 
                    return abstract_syntax_tree_callback< Executor, bool, bool >( from ); 
                };
            using OperationNodeType = std::decay_t< Warp::Utilities::CleanType< decltype( node ) > >;
            return OperationNodeType::operate( 
                    value_from( node.left ), 
                    value_from( node.right ) 
                );
        }
    };

    template< typename InputParameterType, Warp::Parser::ComparisonOperator OperatorParameterConstant >
    struct Executor< bool, InputParameterType, OperatorParameterConstant >
    {
        static bool compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< OperatorParameterConstant >& node )
        {
            const auto value_from = []( const Warp::AbstractSyntaxTree::NodeVariantType& from ) -> typename  { 
                    return abstract_syntax_tree_callback< Executor, bool, InputParameterType >( from ); 
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
