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
        Identifier = 'I', 
        BooleanLiteral = 'b', 
        Unconstrained = 'U', 
        FunctionCall = 'c' 
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

	template<>
    struct Node< Warp::Parser::ExpressionOperator::FactorMultiply >;
	template<>
    struct Node< Warp::Parser::ExpressionOperator::FactorDivide >;
	template<>
    struct Node< Warp::Parser::ExpressionOperator::SumAdd >;
	template<>
    struct Node< Warp::Parser::ExpressionOperator::SumSubtract >;
    
	template<>
    struct Node< NodeType::Literal >;
	template<>
    struct Node< NodeType::BooleanLiteral >;
	template<>
    struct Node< NodeType::Identifier >;
    
	template<>
    struct Node< Warp::Parser::BooleanOperator::LogicalNot >;
	template<>
    struct Node< Warp::Parser::BooleanOperator::LogicalAnd >;
	template<>
    struct Node< Warp::Parser::BooleanOperator::LogicalOr >;
	template<>
    struct Node< Warp::Parser::BooleanOperator::LogicalBiCondition >;
	template<>
    struct Node< Warp::Parser::BooleanOperator::LogicalImplies >;
    
	template<>
    struct Node< Warp::Parser::ComparisonOperator::ComparisonEqual >;
	template<>
    struct Node< Warp::Parser::ComparisonOperator::ComparisonLessThan >;
	template<>
    struct Node< Warp::Parser::ComparisonOperator::ComparisonGreaterThan >;
	template<>
    struct Node< Warp::Parser::ComparisonOperator::ComparisionLessThanOrEqualTo >;
	template<>
    struct Node< Warp::Parser::ComparisonOperator::ComparisionGreaterThanOrEqualTo >;

	template<>
    struct Node< Warp::Parser::FunctionOperators::FunctionResult >;
	template<>
    struct Node< NodeType::FunctionCall >;

	template<>
    struct Node< NodeType::Unconstrained >;

	template<>
	struct Node< Warp::Parser::MetaOperators::MetaOperator >;

    // It would be nice if I did not have to forward declare all of these 🤔 <-- also thats teh first time I have used an unicode emojii in C++ 😊 //
    using InternalNodeVariantType = Utilities::AutoVariant< 

            Node< Warp::Parser::ExpressionOperator::FactorMultiply >, 
            Node< Warp::Parser::ExpressionOperator::FactorDivide >, 
            Node< Warp::Parser::ExpressionOperator::SumAdd >, 
            Node< Warp::Parser::ExpressionOperator::SumSubtract >, 

            Node< NodeType::Literal >, 
            Node< NodeType::BooleanLiteral >, 
            Node< NodeType::Identifier >, 

            Node< Warp::Parser::BooleanOperator::LogicalNot >, 
            Node< Warp::Parser::BooleanOperator::LogicalAnd >, 
            Node< Warp::Parser::BooleanOperator::LogicalOr >, 
            Node< Warp::Parser::BooleanOperator::LogicalBiCondition >, 
            Node< Warp::Parser::BooleanOperator::LogicalImplies >, 

            Node< Warp::Parser::ComparisonOperator::ComparisonEqual >, 
            Node< Warp::Parser::ComparisonOperator::ComparisonLessThan >, 
            Node< Warp::Parser::ComparisonOperator::ComparisonGreaterThan >, 
            Node< Warp::Parser::ComparisonOperator::ComparisionLessThanOrEqualTo >, 
            Node< Warp::Parser::ComparisonOperator::ComparisionGreaterThanOrEqualTo >, 

            Node< Warp::Parser::FunctionOperators::FunctionResult >, 
            Node< NodeType::FunctionCall >, 
            Node< NodeType::Unconstrained >

			Node< Warp::Parser::MetaOperators::MetaOperator >;

        >;

    using NodeVariantType = Utilities::NotSoUniquePointer< InternalNodeVariantType >;


    using LiteralType = StrongFactor< size_t, signed long long int, double, bool >;
    using ValueType = std::variant< size_t, signed long long int, double, bool >;

    struct Taggable
    {
        template< typename NodeTagTypeParameterType >
        constexpr const std::optional< NodeTagTypeParameterType > tag_as() const noexcept 
        {
            return std::optional< NodeTagTypeParameterType >{ std::any_cast< NodeTagTypeParameterType >( type_tag() ) };
        }
        // Includes runtime check, cant do it at compile time :( //
        template< typename NodeTagTypeParameterType >
        const std::optional< NodeTagTypeParameterType > safe_tag_as() const noexcept 
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
            constexpr virtual const std::any type_tag() const noexcept = 0;
    };

    template< auto TagParameterConstant >
    struct BaseNode : public Taggable
    {
        protected: 
            constexpr virtual const std::any type_tag() const noexcept override final {
                return TagParameterConstant;
            }
    };

    template< auto TagParameterConstant >
    struct LeftRight : public BaseNode< TagParameterConstant >
    {
        const NodeVariantType left;
        const NodeVariantType right;
        constexpr LeftRight( const NodeVariantType left, const NodeVariantType right ) noexcept 
                : left( left ), right( right ) {}    
    };

    template< auto TagParameterConstant >
    struct UnaryNode : public BaseNode< TagParameterConstant >
    {
        const NodeVariantType child;
        constexpr UnaryNode( const NodeVariantType child ) noexcept : child( child ) {}    
    };


    template<>
    struct Node< NodeType::Literal > : public BaseNode< NodeType::Literal >
    {
        const LiteralType value;
        constexpr Node( LiteralType value ) noexcept : value( value ) {}
        constexpr Node( std::integral auto value ) noexcept : value( value ) {}
        constexpr Node( Node< NodeType::Literal > const& other ) noexcept : value( other.value ) {}
        constexpr ~Node() = default;
    };

    template<>
    struct Node< NodeType::BooleanLiteral > : public BaseNode< NodeType::BooleanLiteral >
    {
        const bool value;
        constexpr Node( bool value ) noexcept : value( value ) {}
        constexpr Node( Node< NodeType::BooleanLiteral > const& other ) noexcept : value( other.value ) {}
    };

    template<>
    struct Node< Warp::Parser::FunctionOperators::FunctionResult > 
            : public BaseNode< Warp::Parser::FunctionOperators::FunctionResult > { // Does nothing but tell the compiler <insert result of function here> //
        constexpr Node() = default;
        constexpr ~Node() = default;
    };

    template<>
    struct Node< NodeType::Unconstrained > 
            : public BaseNode< NodeType::Unconstrained > { // Does nothing but tell the compiler that the parameter/return value is unconstrained //
        constexpr Node() = default;
        constexpr ~Node() = default;
    };


    template<>
    struct Node< NodeType::Identifier > : public BaseNode< NodeType::Identifier >
    {
        std::string string;
        // const std::optional< Warp::Utilities::HeapStringType > string; // DO NOT CHANGE THE ORDER OF THESE MEMBERS, `value` may depend on the initialization `string` //
        const Warp::Utilities::HashedStringType value; // DO NOT CHANGE THE ORDER OF THESE MEMBERS, `value` may depend on the initialization `string` //
        // constexpr Node( Warp::Utilities::HashedStringType value ) noexcept : string( std::nullopt ), value( value ) {}
        //constexpr 
        Node( std::string_view value ) noexcept : string( value ), value( Warp::Utilities::hash_string( value ) ) {}        
        // constexpr Node( const auto... name_characters ) noexcept 
        //         : string( Warp::Utilities::HeapStringType{ name_characters... } ), // Class members initialized in order of declaration. //
        //         value( Warp::Utilities::hash_string( string->to_string_view() ) ) {} // Class members initialized in order of declaration. //
        //constexpr 
        Node( Node< NodeType::Identifier > const& other ) noexcept : string( other.string ), value( other.value ) {}
        // constexpr ~Node() = default;
    };

    template<>
    struct Node< NodeType::FunctionCall > : public BaseNode< NodeType::FunctionCall >
    {
        Warp::Utilities::HashedStringType identifier;
        std::string function_name;
        std::vector< NodeVariantType > arguments;
        // Warp::Utilities::VectorType< NodeVariantType > arguments;
        Node( 
                Warp::Utilities::HashedStringType identifier, 
                std::string function_name, 
                std::vector< NodeVariantType > arguments 
                // Warp::Utilities::VectorType< NodeVariantType >&& arguments 
            ) noexcept : 
                identifier( identifier ), function_name( function_name ), arguments( arguments ) {}
        ~Node() = default;
    };


    #define DEFINE_UNARY_NODE_CUSTOM_OPERATION( OPERATION_TYPE, VALUE, OPERHAND_TYPE, CUSTOM_OPERATION ) \
        template<> \
        struct Node< VALUE > : public UnaryNode< VALUE > \
        { \
            using BaseType = UnaryNode< VALUE >; \
            constexpr static const auto operate( OPERHAND_TYPE operhand ) noexcept { \
                return CUSTOM_OPERATION( operhand ); \
            } \
            constexpr static OPERATION_TYPE operation = VALUE ; \
            constexpr Node( const NodeVariantType child ) noexcept \
                    : BaseType( child ) {} \
            constexpr Node( Node< VALUE > const& other ) noexcept \
                    : BaseType( other.child ) {} \
            constexpr Node( Node< VALUE >&& other ) noexcept \
                    : BaseType( other.child ) {} \
            constexpr Node& operator=( Node< VALUE > const& other ) noexcept = default; \
            constexpr Node& operator=( Node< VALUE >&& other ) noexcept = default; \
            constexpr ~Node() = default; \
        }

    #define DEFINE_BI_NODE( OPERATION_TYPE, VALUE, LEFT_OPERHAND_TYPE, RIGHT_OPERHAND_TYPE, OPERATOR ) \
        template<> \
        struct Node< VALUE > : public LeftRight< VALUE > \
        { \
            using BaseType = LeftRight< VALUE >; \
            constexpr static const LEFT_OPERHAND_TYPE operate( LEFT_OPERHAND_TYPE left_value, RIGHT_OPERHAND_TYPE right_value ) noexcept { \
                return left_value OPERATOR right_value; \
            } \
            constexpr static OPERATION_TYPE operation = VALUE ; \
            constexpr Node( \
                    const NodeVariantType left, \
                    const NodeVariantType right \
                ) noexcept : BaseType( left, right ) {} \
            constexpr Node( Node< VALUE > const& other ) noexcept \
                    : BaseType( other.left, other.right ) {} \
            constexpr Node( Node< VALUE >&& other ) noexcept \
                    : BaseType( other.left, other.right ) {} \
            constexpr ~Node() = default; \
            constexpr Node& operator=( Node< VALUE > const& other ) noexcept = default; \
            constexpr Node& operator=( Node< VALUE >&& other ) noexcept = default; \
        }

            // template< typename OperhandParameterType > \
            // struct Operator \
            // { \
            //     constexpr Operator() = default; \
            // }; \
            // template< typename OperhandParameterType > \
            // constexpr static const auto operate = Operator< OperhandParameterType >{}; \

            // constexpr auto operate( auto left_value, auto right_value ) noexcept { \
            //     return left_value OPERATOR right_value; \
            // } \
                // std::cout << "#EVAL(Left " << left_value << " " << #OPERATOR << " " << right_value << ")::" << ( left_value OPERATOR right_value ); \

    #define DEFINE_BI_NODE_TEMPLATED_OPERAND( OPERATION_TYPE, VALUE, OPERATOR ) \
        template<> \
        struct Node< VALUE > : public LeftRight< VALUE > \
        { \
            using BaseType = LeftRight< VALUE >; \
            static auto operate( auto left_value, auto right_value ) noexcept { \
                return left_value OPERATOR right_value; \
            } \
            constexpr static OPERATION_TYPE operation = VALUE ; \
            constexpr Node( \
                    const NodeVariantType left, \
                    const NodeVariantType right \
                ) noexcept : BaseType( left, right ) {} \
            constexpr Node( Node< VALUE > const& other ) noexcept \
                    : BaseType( other.left, other.right ) {} \
            constexpr Node( Node< VALUE >&& other ) noexcept \
                    : BaseType( other.left, other.right ) {} \
            constexpr ~Node() = default; \
            constexpr Node& operator=( Node< VALUE > const& other ) noexcept = default; \
            constexpr Node& operator=( Node< VALUE >&& other ) noexcept = default; \
        }


    #define DEFINE_BI_NODE_CUSTOM_OPERATION( OPERATION_TYPE, VALUE, LEFT_OPERHAND_TYPE, RIGHT_OPERHAND_TYPE, CUSTOM_OPERATION ) \
        template<> \
        struct Node< VALUE > : public LeftRight< VALUE > \
        { \
            using BaseType = LeftRight< VALUE >; \
            constexpr static const LEFT_OPERHAND_TYPE operate( LEFT_OPERHAND_TYPE left_value, RIGHT_OPERHAND_TYPE right_value ) noexcept { \
                return CUSTOM_OPERATION( left_value, right_value ); \
            } \
            constexpr static OPERATION_TYPE operation = VALUE ; \
            constexpr Node( \
                    const NodeVariantType left, \
                    const NodeVariantType right \
                ) noexcept : BaseType( left, right ) {} \
            constexpr Node( Node< VALUE > const& other ) noexcept \
                    : BaseType( other.left, other.right ) {} \
            constexpr Node( Node< VALUE >&& other ) noexcept \
                    : BaseType( other.left, other.right ) {} \
            constexpr ~Node() = default; \
            constexpr Node& operator=( Node< VALUE > const& other ) noexcept = default; \
            constexpr Node& operator=( Node< VALUE >&& other ) noexcept = default; \
        }

    #define DEFINE_TRI_NODE_CUSTOM_OPERATION( VALUE_TYPE, TAG_TYPE, TAG, LEFT_OPERHAND_TYPE, RIGHT_OPERHAND_TYPE, CUSTOM_OPERATION ) \
        template<> \
        struct Node< TAG > : public LeftRight< TAG > \
        { \
            using BaseType = LeftRight< TAG >; \
            VALUE_TYPE value; \
            constexpr static const auto operate( LEFT_OPERHAND_TYPE left_value, RIGHT_OPERHAND_TYPE right_value ) noexcept { \
                return CUSTOM_OPERATION( left_value, right_value ); \
            } \
            constexpr static TAG_TYPE operation = TAG ; \
            constexpr Node( \
                    const NodeVariantType left, \
                    const VALUE_TYPE value, \
                    const NodeVariantType right \
                ) noexcept : BaseType( left, right ), value( value ) {} \
            constexpr Node( const Node< TAG >& other ) noexcept \
                    : BaseType( other.left, other.right ), value( other.value ) {} \
            constexpr Node( Node< TAG >&& other ) noexcept \
                    : BaseType( other.left, other.right ), value( other.value ) {} \
            constexpr ~Node() = default; \
            constexpr Node& operator=( const Node< TAG >& other ) noexcept = default; \
            constexpr Node& operator=( Node< TAG >&& other ) noexcept = default; \
        }

    DEFINE_BI_NODE_TEMPLATED_OPERAND( const Warp::Parser::ExpressionOperator, Warp::Parser::ExpressionOperator::FactorMultiply, * );
    DEFINE_BI_NODE_TEMPLATED_OPERAND( const Warp::Parser::ExpressionOperator, Warp::Parser::ExpressionOperator::FactorDivide, / );
    DEFINE_BI_NODE_TEMPLATED_OPERAND( const Warp::Parser::ExpressionOperator, Warp::Parser::ExpressionOperator::SumAdd, + );
    DEFINE_BI_NODE_TEMPLATED_OPERAND( const Warp::Parser::ExpressionOperator, Warp::Parser::ExpressionOperator::SumSubtract, - );

    DEFINE_BI_NODE( const Warp::Parser::BooleanOperator, Warp::Parser::BooleanOperator::LogicalAnd, bool, bool, && );
    DEFINE_BI_NODE( const Warp::Parser::BooleanOperator, Warp::Parser::BooleanOperator::LogicalOr, bool, bool, || );
    DEFINE_BI_NODE( const Warp::Parser::BooleanOperator, Warp::Parser::BooleanOperator::LogicalBiCondition, bool, bool, == );
    DEFINE_BI_NODE_CUSTOM_OPERATION( const Warp::Parser::BooleanOperator, Warp::Parser::BooleanOperator::LogicalImplies, bool, bool, Warp::Utilities::logical_implies );
    DEFINE_UNARY_NODE_CUSTOM_OPERATION( const Warp::Parser::BooleanOperator, Warp::Parser::BooleanOperator::LogicalNot, bool, ! );

    DEFINE_BI_NODE_TEMPLATED_OPERAND( const Warp::Parser::ComparisonOperator, Warp::Parser::ComparisonOperator::ComparisonEqual, == );
    DEFINE_BI_NODE_TEMPLATED_OPERAND( const Warp::Parser::ComparisonOperator, Warp::Parser::ComparisonOperator::ComparisonLessThan, < );
    DEFINE_BI_NODE_TEMPLATED_OPERAND( const Warp::Parser::ComparisonOperator, Warp::Parser::ComparisonOperator::ComparisonGreaterThan, > );
    DEFINE_BI_NODE_TEMPLATED_OPERAND( const Warp::Parser::ComparisonOperator, Warp::Parser::ComparisonOperator::ComparisionLessThanOrEqualTo, <= );
    DEFINE_BI_NODE_TEMPLATED_OPERAND( const Warp::Parser::ComparisonOperator, Warp::Parser::ComparisonOperator::ComparisionGreaterThanOrEqualTo, >= );

    // DEFINE_TRI_NODE_CUSTOM_OPERATION( )
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_TREE_HPP

