#include <Warp/ExpressionTree.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_ABSTRACT_SYNTAX_TREE_UTILITIES_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_ABSTRACT_SYNTAX_TREE_UTILITIES_HPP

namespace Warp::Utilities
{

    template< typename... VariantParameterTypes >
    constexpr const auto& to_const_reference( const Warp::Utilities::NotSoUniquePointer< Warp::Utilities::AutoVariant< VariantParameterTypes... > >& variant ) {
        const typename std::remove_pointer< decltype( variant.get_pointer() ) >::type& node = *variant.get_pointer();
        return node;
    }

    constexpr const auto& to_const_reference( const auto* value ) {
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


    template< std::integral DataLiteralType >
    constexpr Warp::AbstractSyntaxTree::NodeVariantType allocate_integral_literal_node( auto data )
    {
        using NodeInPlaceType = std::in_place_type_t< 
                Warp::AbstractSyntaxTree::Node< 
                        Warp::AbstractSyntaxTree::NodeType::Literal 
                    > 
            >;
        return std::move( Warp::AbstractSyntaxTree::NodeVariantType { 
                std::in_place_type_t< Warp::AbstractSyntaxTree::InternalNodeVariantType >{}, 
                NodeInPlaceType{}, 
                Warp::AbstractSyntaxTree::LiteralType{ Utilities::to_integral< DataLiteralType >( data ) } 
            } );
    }

    constexpr Warp::AbstractSyntaxTree::NodeVariantType allocate_boolean_literal_node( auto data )
    {
        using NodeInPlaceType = std::in_place_type_t< 
                Warp::AbstractSyntaxTree::Node< 
                        Warp::AbstractSyntaxTree::NodeType::BooleanLiteral 
                    > 
            >;
        return Warp::AbstractSyntaxTree::NodeVariantType { 
                std::in_place_type_t< Warp::AbstractSyntaxTree::InternalNodeVariantType >{}, 
                NodeInPlaceType{}, 
                Utilities::to_bool( data, Warp::Parser::true_token, Warp::Parser::false_token ).value() 
            };
    }

    template< auto TagParameterConstant >
    struct NodeTagGetter
    {
        constexpr const static auto tag = TagParameterConstant;
        constexpr NodeTagGetter( std::in_place_type_t< Warp::AbstractSyntaxTree::Node< TagParameterConstant > > ) {}
        constexpr NodeTagGetter( Warp::AbstractSyntaxTree::Node< TagParameterConstant > ) {}
        constexpr NodeTagGetter( Warp::AbstractSyntaxTree::Node< TagParameterConstant >* ) {}
        constexpr NodeTagGetter( const Warp::AbstractSyntaxTree::Node< TagParameterConstant >& ) {}
    };

    template< typename NodeParameterType >
    constexpr static const auto node_tag = decltype( NodeTagGetter( std::in_place_type_t< NodeParameterType >{} ) )::tag;


    template< template< auto > typename BaseParameterType, typename NodeParmaterType >
    constexpr bool node_is_derived_from( NodeParmaterType )
    {
        using NodeType = CleanType< NodeParmaterType >;
        return std::derived_from< NodeType, BaseParameterType< node_tag< NodeType > > >;
    };
    
    template< typename NodeParameterType >
    constexpr bool node_is_left_right( NodeParameterType node ) {
        return node_is_derived_from< Warp::AbstractSyntaxTree::LeftRight, NodeParameterType >( node );
    }

    struct LeftRight
    {
        const Warp::AbstractSyntaxTree::NodeVariantType& left;
        const Warp::AbstractSyntaxTree::NodeVariantType& right;
        template< typename NodeParameterType >
        constexpr LeftRight( NodeParameterType* node ) noexcept : left( node->left ), right( node->right ) {}
    };


    constexpr bool is_bi_node( const Warp::AbstractSyntaxTree::NodeVariantType& from )
    {
        return Warp::Utilities::visit< []( auto* node ) 
                { 
                    if constexpr( node_is_left_right( decltype( node ){} ) == true )
                        return true;
                    return false;
                } >( to_const_reference( from ) );
    }

    constexpr std::optional< LeftRight > bi_node_proxy( const Warp::AbstractSyntaxTree::NodeVariantType& from )
    {
        return Warp::Utilities::visit< []( auto* node ) 
                { 
                    if constexpr( node_is_left_right( decltype( node ){} ) == true )
                        return std::optional{ LeftRight( node ) };
                    return std::optional< LeftRight >{ std::nullopt };
                } >( to_const_reference( from ) );
    }

    // constexpr Warp::AbstractSyntaxTree::NodeVariantType construct_node_of_type( 
    //         const Warp::AbstractSyntaxTree::NodeVariantType& from, 
    //         auto... initializers 
    //     )
    // {
    //     return Warp::Utilities::visit< []( auto* node ) { 
    //                 return allocate_node< node_tag< CleanType< decltype( node ) > > >( initializers... );
    //             } >( to_const_reference( from ) );
    // }

    template< auto NodeParameterTypeConstant >
    constexpr const Warp::AbstractSyntaxTree::NodeVariantType& get_left( const Warp::AbstractSyntaxTree::NodeVariantType& from ) {
        return Warp::Utilities::get_if< Warp::AbstractSyntaxTree::Node< NodeParameterTypeConstant > >( from.get_pointer() )->left;
    }

    template< auto NodeParameterTypeConstant >
    constexpr const Warp::AbstractSyntaxTree::NodeVariantType& get_right( const Warp::AbstractSyntaxTree::NodeVariantType& from ) {
        return Warp::Utilities::get_if< Warp::AbstractSyntaxTree::Node< NodeParameterTypeConstant > >( from.get_pointer() )->right;
    }

    template< auto NodeParameterTypeConstant, typename ToParameterType >
    constexpr const ToParameterType& get_node_value( const Warp::AbstractSyntaxTree::NodeVariantType& from ) {
        // using NodeType = Warp::AbstractSyntaxTree::Node< NodeParameterTypeConstant >;
        // NodeType* ptr = static_cast< NodeType* >( from.get_pointer() );
        return Warp::Utilities::get_if< Warp::AbstractSyntaxTree::Node< NodeParameterTypeConstant > >( from.get_pointer() )->value;
    }

    template< auto NodeTagParameterConstant > 
    constexpr auto tag_is( const Warp::AbstractSyntaxTree::NodeVariantType& node ) {
        return node->data_as< Warp::AbstractSyntaxTree::Taggable, true >()->tag_as< decltype( NodeTagParameterConstant ) >() == NodeTagParameterConstant;
    }
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_ABSTRACT_SYNTAX_TREE_UTILITIES_HPP
