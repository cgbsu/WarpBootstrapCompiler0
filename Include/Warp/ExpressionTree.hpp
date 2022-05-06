#include <Warp/Utilities.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_TREE_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_TREE_HPP

namespace Warp::AbstractSyntaxTree
{

    enum class NodeType : char
    {
        Factor = 'F', 
        Sum = 'S', 
        Comparison = 'C', 
        LogicalOperation = 'B', 
        Literal = 'L', 
        Identifier = 'I' 
    };

    template< typename... ArithmaticParameterTypes >
    struct StrongFactor
    {

        using InternalFactorType = Warp::Utilities::AutoVariant< ArithmaticParameterTypes... >;
        using FactorType = Warp::Utilities::NotSoUniquePointer< InternalFactorType >;
        using ThisType = StrongFactor< ArithmaticParameterTypes... >;
        using ComparisionResultType = std::common_comparison_category_t< std::compare_three_way_result_t< ArithmaticParameterTypes >... >;

        const FactorType factor;
        
        // constexpr StrongFactor( FactorType factor ) : factor( factor ) {}
        template< typename ParameterType >
        constexpr StrongFactor( ParameterType factor_ ) : factor( 
                std::in_place_type_t< InternalFactorType >{}, 
                std::in_place_type_t< typename std::remove_reference< decltype( factor_ ) >::type >{}, 
                factor_ 
            ) {}
        constexpr StrongFactor( const StrongFactor& other ) noexcept = default;
        constexpr StrongFactor( StrongFactor&& other ) noexcept = default;
        constexpr ThisType& operator=( const ThisType& other ) = default;
        constexpr ThisType& operator=( ThisType&& other ) = default;

        constexpr ThisType operator*( const auto other ) {
            return ThisType{ *Warp::Utilities::get_if< decltype( other ) >( factor.get_pointer() ) * other };
        }
        constexpr ThisType operator/( const auto other ) {
            return ThisType{ *Warp::Utilities::get_if< decltype( other ) >( factor.get_pointer() ) / other };
        }
        constexpr ThisType operator+( const auto other ) {
            return ThisType{ *Warp::Utilities::get_if< decltype( other ) >( factor.get_pointer() ) + other };
        }
        constexpr ThisType operator-( const auto other ) {
            return ThisType{ *Warp::Utilities::get_if< decltype( other ) >( factor.get_pointer() ) - other };
        }
        constexpr ComparisionResultType operator<=>( const auto other ) {
            return ( *factor.get_pointer() ).operator<=>( other );
        }
    };

    template< auto >
    struct Node {};

    struct Node< Warp::Parser::ExpressionOperator::FactorMultiply >;
    struct Node< Warp::Parser::ExpressionOperator::FactorDivide >;
    struct Node< Warp::Parser::ExpressionOperator::SumAdd >;
    struct Node< Warp::Parser::ExpressionOperator::SumSubtract >;
    struct Node< NodeType::Literal >;


    using InternalNodeVariantType = Utilities::AutoVariant< 
            Node< Warp::Parser::ExpressionOperator::FactorMultiply >, 
            Node< Warp::Parser::ExpressionOperator::FactorDivide >, 
            Node< Warp::Parser::ExpressionOperator::SumAdd >, 
            Node< Warp::Parser::ExpressionOperator::SumSubtract >, 
            Node< NodeType::Literal > 
        >;

    using NodeVariantType = Utilities::NotSoUniquePointer< InternalNodeVariantType >;


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
        const NodeVariantType left;
        const NodeVariantType right;
        constexpr LeftRight( const NodeVariantType left, const NodeVariantType right ) 
                : left( left ), right( right ) {}    
    };

    template<>
    struct Node< NodeType::Literal > : public BaseNode< NodeType::Literal >
    {
        const LiteralType value;
        constexpr Node( LiteralType value ) noexcept : value( value ) {}
        constexpr Node( std::integral auto value ) noexcept : value( value ) {}
        constexpr Node( Node< NodeType::Literal > const& other ) noexcept : value( other.value ) {}
    };

    template<>
    struct Node< NodeType::Identifier > : public BaseNode< NodeType::Identifier >
    {
        const Warp::Utilities::HeapStringType name;
        constexpr Node( Warp::Utilities::HeapStringType name ) noexcept : name( name ) {}
        constexpr Node( const auto... name_characters ) noexcept : name( name_characters... ) {}
        constexpr Node( Node< NodeType::Identifier > const& other ) noexcept : name( other.name ) {}
    };


    #define DEFINE_BI_NODE( OPERATION_TYPE, VALUE, LEFT_OPERHAND_TYPE, RIGHT_OPERHAND_TYPE, OPERATOR ) \
        template<> \
        struct Node< VALUE > : public LeftRight< VALUE > \
        { \
            using BaseType = LeftRight< VALUE >; \
            constexpr static const auto operate( LEFT_OPERHAND_TYPE left_value, RIGHT_OPERHAND_TYPE right_value ) noexcept { \
                return left_value OPERATOR right_value; \
            } \
            constexpr static OPERATION_TYPE operation = VALUE ; \
            constexpr Node( \
                    const NodeVariantType left, \
                    const NodeVariantType right \
                ) noexcept : BaseType( left, right ) {} \
            constexpr Node( Node< VALUE > const& other ) noexcept \
                    : BaseType( other.left, other.right ) {} \
        }

    #define DEFINE_BI_NODE_CUSTOM_OPERATION( OPERATION_TYPE, VALUE, LEFT_OPERHAND_TYPE, RIGHT_OPERHAND_TYPE, CUSTOM_OPERATION ) \
        template<> \
        struct Node< VALUE > : public LeftRight< VALUE > \
        { \
            using BaseType = LeftRight< VALUE >; \
            constexpr static const auto operate( LEFT_OPERHAND_TYPE left_value, RIGHT_OPERHAND_TYPE right_value ) noexcept { \
                return CUSTOM_OPERATION( left_value, right_value ); \
            } \
            constexpr static OPERATION_TYPE operation = VALUE ; \
            constexpr Node( \
                    const NodeVariantType left, \
                    const NodeVariantType right \
                ) noexcept : BaseType( left, right ) {} \
            constexpr Node( Node< VALUE > const& other ) noexcept \
                    : BaseType( other.left, other.right ) {} \
        }


    DEFINE_BI_NODE( const Warp::Parser::ExpressionOperator, Warp::Parser::ExpressionOperator::FactorMultiply, auto, auto, * );
    DEFINE_BI_NODE( const Warp::Parser::ExpressionOperator, Warp::Parser::ExpressionOperator::FactorDivide, auto, auto, / );
    DEFINE_BI_NODE( const Warp::Parser::ExpressionOperator, Warp::Parser::ExpressionOperator::SumAdd, auto, auto, + );
    DEFINE_BI_NODE( const Warp::Parser::ExpressionOperator, Warp::Parser::ExpressionOperator::SumSubtract, auto, auto, - );
    DEFINE_BI_NODE( const Warp::Parser::BooleanOperator, Warp::Parser::BooleanOperator::LogicalAnd, bool, bool, && );
    DEFINE_BI_NODE( const Warp::Parser::BooleanOperator, Warp::Parser::BooleanOperator::LogicalOr, bool, bool, || );
    DEFINE_BI_NODE( const Warp::Parser::BooleanOperator, Warp::Parser::BooleanOperator::LogicalBiConditional, bool, bool, == );
    DEFINE_BI_NODE_CUSTOM_OPERATION( const Warp::Parser::BooleanOperator, Warp::Parser::BooleanOperator::LogicalImplies, bool, bool, Warp::Utilities::logical_implies );
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_TREE_HPP
