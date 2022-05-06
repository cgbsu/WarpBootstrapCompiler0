#include <Warp/ExpressionTree.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_ABSTRACT_SYNTAX_TREE_UTILITIES_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_ABSTRACT_SYNTAX_TREE_UTILITIES_HPP

namespace Warp::Utilities
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
    
    template< auto NodeTag >
    constexpr Warp::AbstractSyntaxTree::NodeVariantType allocate_node( auto... constructor_arguments )
    {
        using NodeInPlaceType = std::in_place_type_t< 
                Warp::AbstractSyntaxTree::Node< 
                        NodeTag 
                    > 
            >;
        return Warp::AbstractSyntaxTree::NodeVariantType ( 
                std::in_place_type_t< Warp::AbstractSyntaxTree::InternalNodeVariantType >{}, 
                NodeInPlaceType{}, 
                constructor_arguments... 
            );
    }


    template< typename DataLiteralType >
    constexpr Warp::AbstractSyntaxTree::NodeVariantType allocate_integral_literal_node( auto data )
    {
        using NodeInPlaceType = std::in_place_type_t< 
                Warp::AbstractSyntaxTree::Node< 
                        Warp::AbstractSyntaxTree::NodeType::Literal 
                    > 
            >;
        return Warp::AbstractSyntaxTree::NodeVariantType { 
                std::in_place_type_t< Warp::AbstractSyntaxTree::InternalNodeVariantType >{}, 
                NodeInPlaceType{}, 
                Warp::AbstractSyntaxTree::LiteralType{ Utilities::to_integral< DataLiteralType >( data ) } 
            };
    }

}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_ABSTRACT_SYNTAX_TREE_UTILITIES_HPP
