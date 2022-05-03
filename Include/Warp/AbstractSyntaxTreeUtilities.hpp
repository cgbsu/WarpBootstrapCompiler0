#include <Warp/ExpressionTree.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_ABSTRACT_SYNTAX_TREE_UTILITIES_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_ABSTRACT_SYNTAX_TREE_UTILITIES_HPP

namespace Warp::Utilities
{

    template< auto NodeTag >
    constexpr Warp::AbstractSyntaxTree::VariantType allocate_node( auto... constructor_arguments )
    {
        return Warp::AbstractSyntaxTree::VariantType { 
                new Warp::AbstractSyntaxTree::InternalVariantType { 
                        std::in_place_type_t< Warp::AbstractSyntaxTree::Node< NodeTag > >{}, 
                        constructor_arguments... 
                    }
            };
    }


    template< typename DataLiteralType >
    constexpr Warp::AbstractSyntaxTree::VariantType allocate_integral_literal_node( auto data )
    {
        using NodeInPlaceType = std::in_place_type_t< 
                Warp::AbstractSyntaxTree::Node< 
                        Warp::AbstractSyntaxTree::NodeType::Literal 
                    > 
            >;
        using NoRef = DataLiteralType;//std::remove_reference< DataLiteralType >::type;
        auto literal = Warp::AbstractSyntaxTree::LiteralType { 
                Utilities::to_integral< NoRef >( data ) 
            };
        return Warp::AbstractSyntaxTree::VariantType { 
                new Warp::AbstractSyntaxTree::InternalVariantType { 
                        NodeInPlaceType{}, 
                        literal 
                    }
            };
    }

    template< typename = void >
    struct StringableTrait
    {
        virtual std::string_view to_string() /*( const size_t tabs )*/ const = 0;
        /*std::string_view make_tabs( const size_t tabs ) const
        {
            std::stringstream tab_buffer;
            for( size_t ii = 0; ii < tabs; ++ii )
                tab_buffer << "\t";
            return std::string_view{ tab_buffer.str() };
        }*/
    };

    std::string_view tree_to_string( const Warp::AbstractSyntaxTree::VariantType node );//, const size_t tabs );

    #define DEFINE_STRINGABLE_FOR_BI_NODE_TYPE( TAG_VALUE ) \
        template<> \
        struct StringableTrait< Warp::AbstractSyntaxTree::Node< TAG_VALUE >* > : public StringableTrait< void > \
        { \
            const Warp::AbstractSyntaxTree::Node< TAG_VALUE >* node; \
            constexpr StringableTrait( Warp::AbstractSyntaxTree::Node< TAG_VALUE >* node ) : node( node ) {} \
            virtual std::string_view to_string() const override final \
            { \
                std::string_view child_nodes[] = { \
                        tree_to_string( node->left ), \
                        tree_to_string( node->right ) \
                    }; \
                std::stringstream buffer; \
                buffer \
                   /* << tabs*/ << "Node< " << Utilities::to_string< decltype( TAG_VALUE ) >( TAG_VALUE ).to_string_view() << ">{\n" \
                   /* << tabs*/ << "\t" << "Left{\n" << child_nodes[ 0 ] \
                   /* << tabs*/ << "\t}\n" \
                   /* << tabs*/ << "\t" << "Right{\n" \
                   /* << tabs*/ << "\t" << child_nodes[ 1 ] \
                   /* << tabs*/ << "\t}\n" \
                   /* << tabs*/ << "}\n"; \
                   return NotSoUniquePointer< const char* >{ std::string_view{ buffer.str() }.data() }; \
            } \
        }

                // std::string_view tabs = make_tabs( tabs_ ); \
                // return std::string_view{ buffer.str() }; 

    DEFINE_STRINGABLE_FOR_BI_NODE_TYPE( Parser::ExpressionOperator::FactorMultiply );
    DEFINE_STRINGABLE_FOR_BI_NODE_TYPE( Parser::ExpressionOperator::FactorDivide );
    DEFINE_STRINGABLE_FOR_BI_NODE_TYPE( Parser::ExpressionOperator::SumAdd );
    DEFINE_STRINGABLE_FOR_BI_NODE_TYPE( Parser::ExpressionOperator::SumSubtract );

    template<>
    struct StringableTrait< Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Literal >* > : public StringableTrait< void >
    {
        const Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Literal >* node;
        constexpr StringableTrait( Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::Literal >* node ) : node( node ) {}
        virtual std::string_view to_string() /*( const size_t tabs )*/ const override final 
        {
            std::stringstream buffer;
            buffer 
                    // << make_tabs( tabs ) 
                    << "Node< " << Utilities::to_string< Warp::AbstractSyntaxTree::NodeType >( 
                            Warp::AbstractSyntaxTree::NodeType::Literal ).to_string_view() 
                    << ">{" << node->value.to_string() << "}\n";
            // return std::string_view{ buffer.str() };
                   return NotSoUniquePointer< const char* >{ std::string_view{ buffer.str() }.data() };
        }
    };


    std::string_view tree_to_string( const Warp::AbstractSyntaxTree::VariantType node )
    {
        StringableTrait< void >* stringable = Utilities::visit< []( auto underyling_node ) {
                return new StringableTrait< decltype( underyling_node ) >{ underyling_node };//.to_string();
            } >( node.get_pointer() );//*node.get_pointer() );
        std::string_view string = stringable->to_string();
        delete stringable;
        return string;
    };

}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_ABSTRACT_SYNTAX_TREE_UTILITIES_HPP
