#include <Warp/CtpgUtilities.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_TREE_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_TREE_HPP

namespace Warp::AbstractSyntaxTree
{

    enum class NodeType : char
    {
        Factor = 'F', 
        Sum = 'S', 
        Literal = 'L' 
    };

    template< typename... ArithmaticParameterTypes >
    struct StrongFactor : public Warp::Utilities::ConstexprStringable
    {
        using FactorType = std::variant< ArithmaticParameterTypes... >;
        using ThisType = StrongFactor< ArithmaticParameterTypes... >;
        using ComparisionResultType = std::common_comparison_category_t< std::compare_three_way_result_t< ArithmaticParameterTypes >... >;

        const FactorType factor;
        
        constexpr StrongFactor( FactorType factor ) : factor( factor ) {}
        constexpr StrongFactor( auto factor ) : factor( factor ) {}

        constexpr virtual std::string_view to_string() const override final {
            return Utilities::to_string( factor );
        }

        constexpr ThisType operator*( const auto other ) {
            return ThisType{ *std::get_if< decltype( other ) >( &factor ) * other };
        }
        constexpr ThisType operator/( const auto other ) {
            return ThisType{ *std::get_if< decltype( other ) >( &factor ) / other };
        }
        constexpr ThisType operator+( const auto other ) {
            return ThisType{ *std::get_if< decltype( other ) >( &factor ) + other };
        }
        constexpr ThisType operator-( const auto other ) {
            return ThisType{ *std::get_if< decltype( other ) >( &factor ) - other };
        }
        constexpr ComparisionResultType operator<=>( const auto other ) {
            return factor.operator<=>( other );
        }
    };

    template< auto >
    struct Node {};

    struct Node< Warp::Parser::ExpressionOperator::FactorMultiply >;
    struct Node< Warp::Parser::ExpressionOperator::FactorDivide >;
    struct Node< Warp::Parser::ExpressionOperator::SumAdd >;
    struct Node< Warp::Parser::ExpressionOperator::SumSubtract >;
    struct Node< NodeType::Literal >;


    using InternalVariantType = Utilities::AutoVariant< 
            Node< Warp::Parser::ExpressionOperator::FactorMultiply >, 
            Node< Warp::Parser::ExpressionOperator::FactorDivide >, 
            Node< Warp::Parser::ExpressionOperator::SumAdd >, 
            Node< Warp::Parser::ExpressionOperator::SumSubtract >, 
            Node< NodeType::Literal > 
        >;

    using VariantType = Utilities::NotSoUniquePointer< InternalVariantType >;


    using LiteralType = StrongFactor< size_t, signed long long int >;

    struct Taggable
    {
        template< typename NodeTagTypeParameterType >
        constexpr const std::optional< NodeTagTypeParameterType > tag_as() const
        {
            if constexpr( 
                        auto result = std::any_cast< NodeTagTypeParameterType >( type_tag ); 
                        result != nullptr 
                    )
                return std::optional{ result };
            return std::nullopt;
        }
        // Includes runtime check, cant do it at compile time :( //
        template< typename NodeTagTypeParameterType >
        const std::optional< NodeTagTypeParameterType > safe_tag_as() const
        {
            if( tag_type_id() == typeid( NodeTagTypeParameterType ).hash_code() )
                return tag_as< NodeTagTypeParameterType >();
            return std::nullopt;
        }
        // Runtime function //
        size_t tag_type_id() const {
            return type_tag().type().hash_code();
        }
        protected: 
            constexpr virtual const std::any type_tag() const = 0;
    };

    template< auto TagParameterConstant >
    struct BaseNode : public Taggable
    {
        protected: 
            constexpr virtual const std::any type_tag() const override final {
                return TagParameterConstant;
            }
    };

    template< auto TagParameterConstant >
    struct LeftRight : public BaseNode< TagParameterConstant >
    {
        const VariantType left;
        const VariantType right;
        constexpr LeftRight( const VariantType left, const VariantType right ) 
                : left( left ), right( right ) {}    
    };

    template<>
    struct Node< NodeType::Literal > : public BaseNode< NodeType::Literal >
    {
        const LiteralType value;
        constexpr Node( LiteralType value ) : value( value ) {}
        constexpr Node( auto value ) : value( value ) {}
        constexpr Node( Node< NodeType::Literal > const& other ) : value( other.value ) {}
    };


    #define DEFINE_BI_NODE( VALUE ) \
        template<> \
        struct Node< VALUE > : public LeftRight< VALUE > \
        { \
            using BaseType = LeftRight< VALUE >; \
            constexpr static const Warp::Parser::ExpressionOperator operation = VALUE ; \
            constexpr Node( \
                    const VariantType left, \
                    const VariantType right \
                ) : BaseType( left, right ) {} \
            constexpr Node( Node< VALUE > const& other ) \
                    : BaseType( other.left, other.right ) {} \
        }

    DEFINE_BI_NODE( Warp::Parser::ExpressionOperator::FactorMultiply );
    DEFINE_BI_NODE( Warp::Parser::ExpressionOperator::FactorDivide );
    DEFINE_BI_NODE( Warp::Parser::ExpressionOperator::SumAdd );
    DEFINE_BI_NODE( Warp::Parser::ExpressionOperator::SumSubtract );
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_TREE_HPP
