#include <Warp/ExpressionTree.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_ABSTRACT_SYNTAX_TREE_UTILITIES_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_ABSTRACT_SYNTAX_TREE_UTILITIES_HPP

namespace Warp::Utilities
{

    template< auto NodeTag >
    constexpr Warp::AbstractSyntaxTree::VariantType allocate_node( auto... constructor_arguments )
    {
        using NodeInPlaceType = std::in_place_type_t< 
                Warp::AbstractSyntaxTree::Node< 
                        NodeTag 
                    > 
            >;
        return Warp::AbstractSyntaxTree::VariantType ( 
                std::in_place_type_t< Warp::AbstractSyntaxTree::InternalVariantType >{}, 
                NodeInPlaceType{}, 
                constructor_arguments... 
            );
    }


    template< typename DataLiteralType >
    constexpr Warp::AbstractSyntaxTree::VariantType allocate_integral_literal_node( auto data )
    {
        using NodeInPlaceType = std::in_place_type_t< 
                Warp::AbstractSyntaxTree::Node< 
                        Warp::AbstractSyntaxTree::NodeType::Literal 
                    > 
            >;
        return Warp::AbstractSyntaxTree::VariantType { 
                std::in_place_type_t< Warp::AbstractSyntaxTree::InternalVariantType >{}, 
                NodeInPlaceType{}, 
                Warp::AbstractSyntaxTree::LiteralType{ Utilities::to_integral< DataLiteralType >( data ) } 
            };
    }

}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_ABSTRACT_SYNTAX_TREE_UTILITIES_HPP
