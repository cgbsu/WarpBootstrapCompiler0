#include <Warp/AbstractSyntaxTreeUtilities.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_ANYLSIS_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_ANYLSIS_HPP

namespace Warp::Analysis
{
    template< typename... VariantParameterTypes >
    const auto& to_const_reference( const Warp::Utilities::NotSoUniquePointer< Warp::Utilities::AutoVariant< VariantParameterTypes... > >& variant ) {
        const typename std::remove_pointer< decltype( variant.get_pointer() ) >::type& node = *variant.get_pointer();
        return node;
    }

    const auto& to_const_reference( const auto* value ) {
        const typename std::remove_pointer< decltype( value ) >::type& node = *value;
        return node;
    }


    template< auto NodeTypeParameterConstant >
    struct ExtractNodeType {
        constexpr static auto node_type = NodeTypeParameterConstant;
        ExtractNodeType( const Warp::AbstractSyntaxTree::Node< NodeTypeParameterConstant >& ) {}
    };

    template< template< typename, auto > typename FeedbackParameterType, typename ReturnParameterType >
    ReturnParameterType abstract_syntax_tree_callback( const Warp::AbstractSyntaxTree::NodeVariantType& variant )
    {
        const auto& node = to_const_reference( variant );
        return Warp::Utilities::visit< []( auto raw_node_pointer ) { 
               const decltype( *raw_node_pointer )& ref = *raw_node_pointer;
               return FeedbackParameterType< ReturnParameterType, decltype( ExtractNodeType( ref ) )::node_type >::compute_value_of_expression( ref ); 
            }>( node );
    }

    template< typename, auto >
    struct Computer{};


    template< typename ExpressionParameterType, Warp::AbstractSyntaxTree::NodeType NodeTypeParameterConstant >
    struct Computer< ExpressionParameterType, NodeTypeParameterConstant >
    {
        static ExpressionParameterType compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< NodeTypeParameterConstant >& node )
        {
            return Warp::Utilities::visit< []( auto value_pointer ) -> ExpressionParameterType {
                    return *value_pointer;
                } >( to_const_reference( node.value.factor ) );
        }
    };

    template< typename ExpressionParameterType, Warp::Parser::ExpressionOperator OperatorParameterConstant >
    struct Computer< ExpressionParameterType, OperatorParameterConstant >
    {
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
