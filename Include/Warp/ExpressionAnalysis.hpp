#include <Warp/AbstractSyntaxTreeUtilities.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_ANYLSIS_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_ANYLSIS_HPP

namespace Warp::Analysis
{
    const auto& to_const_reference( const auto& variant ) {
        const typename std::remove_pointer< decltype( variant.get_pointer() ) >::type& node = *variant.get_pointer();
        return node;
    }

    template< auto NodeTypeParameterConstant >
    struct ExtractNodeType {
        constexpr static auto node_type = NodeTypeParameterConstant;
        ExtractNodeType( const Warp::AbstractSyntaxTree::Node< NodeTypeParameterConstant >& ) {}
    };

    template< template< typename, auto > typename FeedbackParameterType, typename ReturnParameterType >
    ReturnParameterType abstract_syntax_tree_callback( const Warp::AbstractSyntaxTree::VariantType& variant )
    {
        const auto& node = to_const_reference( variant );
        //Soooo... I have to fix this later... its pretty gross... //
        // const typename std::remove_pointer< decltype( variant.get_pointer() ) >::type& node = variant.get_pointer();
        return Warp::Utilities::visit< []( auto raw_node_pointer ) { 
            std::cout << "abstract_syntax_tree_callback got: " << Warp::Utilities::friendly_type_name< decltype( *raw_node_pointer ) >() << "\n";
               const decltype( *raw_node_pointer )& ref = *raw_node_pointer;
               return FeedbackParameterType< ReturnParameterType, decltype( ExtractNodeType( ref ) )::node_type >::compute_value_of_expression( ref ); 
            }>( node );
    }

    // template< auto NodeTypeParameterConstant >
    // concept StructuralNodeValeConcept = 
    //         NodeTypeParameterConstant == Warp::Parser::ExpressionOperator::FactorMultiply 
    //         || NodeTypeParameterConstant == Warp::Parser::ExpressionOperator::FactorDivide
    //         || NodeTypeParameterConstant == Warp::Parser::ExpressionOperator::SumAdd
    //         || NodeTypeParameterConstant == Warp::Parser::ExpressionOperator::SumSubtract;

    template< typename, auto >
    struct Memes{};


    template< typename ExpressionParameterType, Warp::AbstractSyntaxTree::NodeType NodeTypeParameterConstant >
    struct Memes< ExpressionParameterType, NodeTypeParameterConstant >
    {
        static auto compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< NodeTypeParameterConstant >& node )
        // Warp::AbstractSyntaxTree::NodeType::Literal >& node )
        {
            using FactorType = Warp::Utilities::CleanType< decltype( *node.value.factor.get_pointer() ) >;
            size_t size_t_index = FactorType::template type_index< size_t >;
            size_t long_long_int_index = FactorType::template type_index< signed long long int >;
            std::cout << "TEST: " << size_t_index << "\n";
            std::cout << "TEST: " << long_long_int_index << "\n";
            std::cout << "INDEX: " << node.value.factor->index() << "\n";
                // std::cout << "Test: " << Warp::Utilities::friendly_type_name< std::decay_t< size_t > >() << "\n";
                // std::cout << "Test: " << Warp::Utilities::friendly_type_name< std::decay_t< signed long long int > >() << "\n";
            return Warp::Utilities::visit< []( auto value_pointer ) {
                std::cout << "compute_value_of_expression got: " << Warp::Utilities::friendly_type_name< decltype( *value_pointer ) >() << "\n";
                    ExpressionParameterType data = *value_pointer;
                    return data; //is_of_type< ExpressionParameterType >( *value_pointer ); 
                    // return *value_pointer;
                } >( to_const_reference( node.value.factor ) );
            std::cout << "Null? " << ( ( node.value.factor->get_data() == nullptr ) ? "Yes" : "No" ) << "\n";
            // return 0;
        }
    };

    template< typename ExpressionParameterType, Warp::Parser::ExpressionOperator OperatorParameterConstant >
    struct Memes< ExpressionParameterType, OperatorParameterConstant >
    {
        static auto compute_value_of_expression( const Warp::AbstractSyntaxTree::Node< OperatorParameterConstant >& node )
        {
            const auto value_from = []( const Warp::AbstractSyntaxTree::VariantType& from ) { 
                    return abstract_syntax_tree_callback< Memes, ExpressionParameterType >( from ); 
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
