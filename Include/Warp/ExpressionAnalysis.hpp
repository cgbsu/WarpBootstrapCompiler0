#include <Warp/AbstractSyntaxTreeUtilities.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_ANYLSIS_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_ANYLSIS_HPP

namespace Warp::Analysis
{
    const auto& to_const_reference( auto variant ) {
        const typename std::remove_pointer< decltype( variant.get_pointer() ) >::type& node = *variant.get_pointer();
        return node;
    }

    template< auto FeedbackParameterConstant >
    auto abstract_syntax_tree_callback( const Warp::AbstractSyntaxTree::VariantType& variant )
    {
        const auto& node = to_const_reference( variant );
        return Warp::Utilities::visit< []( auto raw_node_pointer ) { 
                return FeedbackParameterConstant( *raw_node_pointer ); 
            }>( node );
    }

    template< typename ExpressionParameterType >
    auto compute_value_of_expression( const auto& node )
    {
        const auto value_from = []( auto from ) { 
                return abstract_syntax_tree_callback< compute_value_of_expression< ExpressionParameterType > >( from ); 
            };
        return decltype( node )::operation( 
                value_from( node.left ), 
                value_from( node.right ) 
            );
    }

    template< typename QueryParameterType >
    auto is_of_type( auto canidate )
    {
        static_assert( 
                std::is_same< QueryParameterType, decltype( canidate ) >::value, 
                "Error:: Attempt to parse literal in expression with a type that differed than the parsed literal type" 
            );
        return canidate;
    }

    template< typename ExpressionParameterType >
    auto compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Literal >& node )
    {
        return Warp::Utilities::visit< [ & ]( auto value_pointer ) { 
                return is_of_type< ExpressionParameterType >( *value_pointer ); 
            } >( to_const_reference( node.value.factor ) );
    }
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_HPP
