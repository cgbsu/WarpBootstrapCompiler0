#include <Warp/CtpgUtilities.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_TREE_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_TREE_HPP

namespace Warp::AbstractSyntaxTree
{

    enum class NodeType : char
    {
        Factor = 'F', 
        Sum = 'S', 
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
        constexpr Node( LiteralType value ) noexcept : value( value ) {}
        constexpr Node( std::integral auto value ) noexcept : value( value ) {}
        constexpr Node( Node< NodeType::Literal > const& other ) noexcept : value( other.value ) {}
    };

    template<>
    struct Node< NodeType::Identifier > : public BaseNode< NodeType::Identifier >
    {
        const LiteralType value;
        constexpr Node( LiteralType value ) noexcept : value( value ) {}
        constexpr Node( std::integral auto value ) noexcept : value( value ) {}
        constexpr Node( Node< NodeType::Literal > const& other ) noexcept : value( other.value ) {}
    };


    #define DEFINE_BI_NODE( VALUE, OPERATOR ) \
        template<> \
        struct Node< VALUE > : public LeftRight< VALUE > \
        { \
            using BaseType = LeftRight< VALUE >; \
            constexpr static const auto operate( auto left_value, auto right_value ) noexcept { \
                return left_value OPERATOR right_value; \
            } \
            constexpr static const Warp::Parser::ExpressionOperator operation = VALUE ; \
            constexpr Node( \
                    const VariantType left, \
                    const VariantType right \
                ) noexcept : BaseType( left, right ) {} \
            constexpr Node( Node< VALUE > const& other ) noexcept \
                    : BaseType( other.left, other.right ) {} \
        }

    DEFINE_BI_NODE( Warp::Parser::ExpressionOperator::FactorMultiply, * );
    DEFINE_BI_NODE( Warp::Parser::ExpressionOperator::FactorDivide, / );
    DEFINE_BI_NODE( Warp::Parser::ExpressionOperator::SumAdd, + );
    DEFINE_BI_NODE( Warp::Parser::ExpressionOperator::SumSubtract, - );
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_TREE_HPP
