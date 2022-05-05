#include <Warp/ExpressionAnaysis.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_EXECUTOR_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_EXECUTOR_HPP

namespace Warp::Execution
{
    // Keeping this in mind... may add later... 
    // enum class TreeType : size_t {
    //     Expression = 0, 
    //     Constraint = 1 
    // };

    // template< auto ExecutorTypeParameterConstant > 
    // struct Executor {};

    template< typename, auto >
    struct Executor{};


    template< typename ExpressionParameterType, Warp::AbstractSyntaxTree::NodeType NodeTypeParameterConstant >
    struct Executor< ExpressionParameterType, NodeTypeParameterConstant >
    {
        static ExpressionParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< NodeTypeParameterConstant >& node )
        {
            return Warp::Utilities::visit< []( auto value_pointer ) -> ExpressionParameterType {
                    return *value_pointer;
                } >( to_const_reference( node.value.factor ) );
        }
    };


    template< typename ExpressionParameterType, Warp::Parser::ExpressionOperator OperatorParameterConstant >
    struct Executor< ExpressionParameterType, OperatorParameterConstant >
    {
        template< typename... FunctionParameterTypes >
        static ExpressionParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< OperatorParameterConstant >& node )
        {
            const auto value_from = []( const Warp::AbstractSyntaxTree::NodeVariantType& from ) -> ExpressionParameterType { 
                    return abstract_syntax_tree_callback< Computer, ExpressionParameterType >( from ); 
                };
            using OperationNodeType = std::decay_t< Warp::Utilities::CleanType< decltype( node ) > >;
            return OperationNodeType::operate( 
                    value_from( node.left ), 
                    value_from( node.right ) 
                );

            []( Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Identifier >* identifier ) {

            }

            return Warp::Utilities::visit< []( auto value_pointer ) -> ExpressionParameterType {
                    return *value_pointer;
                } >( to_const_reference( node.value.factor ) );

        }
    };


    auto make_executor( Warp::AbstractSyntaxTree::NodeVariantType )
    {
        return 
    }
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_EXECUTOR_HPP
