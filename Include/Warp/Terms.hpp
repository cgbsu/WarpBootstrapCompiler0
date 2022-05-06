#include <Warp/Function.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_TERMS_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_TERMS_HPP


namespace Warp::Parser
{
    enum class RegexLiteralTerms {
        NaturalNumber = 0, 
        Identifier = 1
    };

    enum class NonTerminalTerms
    {
        Factor = 0, 
        Sum = 1, 
        ParenthesisScope = 2, 
        Comparison = 3, 
        Negation = 4, 
        LogicalOperation = 5 
    };

    enum class StringTerms
    {
        LetKeyword = 0, 
        And = 1, 
        Or = 2, 
        Not = 3 
    };

    template< auto LiteralParameterTypeParameterConstant >
    struct Term {};

    template<>
    struct Term< RegexLiteralTerms::NaturalNumber >
    {
        constexpr const static char regex[] = "[0-9][0-9]*";
        constexpr const static char name[] = "NaturalNumber";
        constexpr const static auto term = ctpg::regex_term< regex >{ name };
        constexpr const static auto literal_type = RegexLiteralTerms::NaturalNumber;
    };

    template<>
    struct Term< RegexLiteralTerms::Identifier >
    {
        constexpr const static char regex[] = "[a-zA-Z\\_][a-zA-Z0-9\\_]*";
        constexpr const static char name[] = "Identifier";
        constexpr const static auto term = ctpg::regex_term< regex >{ name };
        constexpr const static auto literal_type = RegexLiteralTerms::Identifier;
    };

    template< auto TypeParameterConstant = RegexLiteralTerms::NaturalNumber >
    struct DefaultTypes {
        using Type = size_t;
    };

    template<>
    struct DefaultTypes< RegexLiteralTerms::Identifier > {
        using Type = Warp::Utilities::HeapStringType;
    };

    // Convert enum value to string? I know there is a library to do this.
    template<>
    struct Term< NonTerminalTerms::Factor >
    {
        using StorageType = Warp::AbstractSyntaxTree::NodeVariantType;
        constexpr static const char name[] = "Factor";
        constexpr static const auto term = ctpg::nterm< StorageType >( name );
    };


    template<>
    struct Term< NonTerminalTerms::Sum >
    {
        using StorageType = Warp::AbstractSyntaxTree::NodeVariantType;
        constexpr static const char name[] = "Sum";
        constexpr static const auto term = ctpg::nterm< StorageType >( name );
    };

    template<>
    struct Term< NonTerminalTerms::ParenthesisScope >
    {
        using StorageType = Warp::AbstractSyntaxTree::NodeVariantType;
        constexpr static const char name[] = "ParenthesisScope";
        constexpr static const auto term = ctpg::nterm< StorageType >( name );
    };

    template<>
    struct Term< NonTerminalTerms::And >
    {
        using StorageType = Warp::AbstractSyntaxTree::NodeVariantType;
        constexpr static const char name[] = "And";
        constexpr static const auto term = ctpg::nterm< StorageType >( name );
    };

    template<>
    struct Term< NonTerminalTerms::Or >
    {
        using StorageType = Warp::AbstractSyntaxTree::NodeVariantType;
        constexpr static const char name[] = "Or";
        constexpr static const auto term = ctpg::nterm< StorageType >( name );
    };

    template< 
            auto PriorityParameterConstant, 
            typename TermParameterType, 
            TermParameterType RawTermParameterConstant 
        >
    auto forward_term() {
        return 0;
    }

    template< auto PriorityParameterConstant, auto RawTermParameterConstant >
    auto forward_term() {
        return 0;
    }

    template< auto PriorityParameterConstant, auto... TermParameterConstants >
    struct TermBuilder
    {
        constexpr static const auto priority = PriorityParameterConstant;
    };
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_TERMS_HPP
