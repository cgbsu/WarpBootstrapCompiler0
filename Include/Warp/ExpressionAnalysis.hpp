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
            std::cout << Warp::Utilities::type_name< decltype( *raw_node_pointer ) >();
                const decltype( *raw_node_pointer )& ref = *raw_node_pointer;
                return FeedbackParameterConstant( ref ); 
            }>( node );
    }

    template< typename ExpressionParameterType >
    auto compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Literal >& node )
    {
        // return Warp::Utilities::visit< [ & ]( const auto& value_pointer ) { 
        //         return *value_pointer; //is_of_type< ExpressionParameterType >( *value_pointer ); 
        //     } >( to_const_reference( node.value.factor ) );
        return 0;
    }

    template< typename ExpressionParameterType, Warp::Parser::ExpressionOperator OperatorParameterConstant >
    auto compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< OperatorParameterConstant >& node )
    {
        const auto value_from = []( const Warp::AbstractSyntaxTree::VariantType& from ) { 
                return abstract_syntax_tree_callback< compute_value_of_expression< ExpressionParameterType > >( from ); 
            };
        using OperationNodeType = std::decay_t< Warp::Utilities::CleanType< decltype( node ) > >;
        return OperationNodeType::operation( 
                value_from( node.left ), 
                value_from( node.right ) 
            );
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

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_HPP
