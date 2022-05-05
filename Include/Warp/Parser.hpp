#include <Warp/Terms.hpp>
#include <ctpg.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP

namespace Warp::Parser
{
    enum class RegexLiteralTerms {
        NaturalNumber = 0, 
        Identifier = 1
    };

    enum class NonTerminalTerms {
        Factor = 0, 
        Sum = 1, 
        ParenthesisScope = 2, 
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
        using StorageType = Warp::AbstractSyntaxTree::VariantType;
        constexpr static const char name[] = "Factor";
        constexpr static const auto term = ctpg::nterm< StorageType >( name );
    };

    template<>
    struct Term< NonTerminalTerms::Sum >
    {
        using StorageType = Warp::AbstractSyntaxTree::VariantType;
        constexpr static const char name[] = "Sum";
        constexpr static const auto term = ctpg::nterm< StorageType >( name );
    };

    template<>
    struct Term< NonTerminalTerms::ParenthesisScope >
    {
        using StorageType = Warp::AbstractSyntaxTree::VariantType;
        constexpr static const char name[] = "ParenthesisScope";
        constexpr static const auto term = ctpg::nterm< StorageType >( name );
    };


    template< template< auto > typename TypeResolverParameterType = DefaultTypes >
    struct WarpParser
    {
        template< auto ParameterConstant >
        constexpr const static auto term = Term< ParameterConstant >::term;

        constexpr static const auto factor = term< NonTerminalTerms::Factor >;
        constexpr static const auto sum = term< NonTerminalTerms::Sum >;
        constexpr static const auto parenthesis_scope = term< NonTerminalTerms::ParenthesisScope >;

        // TODO: Find a nice way to set up operatore precedence.
        constexpr static const auto plus_term = ctpg::char_term( Warp::Utilities::to_char( 
                    ExpressionOperator::SumAdd ), 1, ctpg::associativity::ltor 
            );
        constexpr static const auto minus_term = ctpg::char_term( Warp::Utilities::to_char( 
                    ExpressionOperator::SumSubtract ), 1, ctpg::associativity::ltor 
            );
        constexpr static const auto multiply_term = ctpg::char_term( Warp::Utilities::to_char( 
                    ExpressionOperator::FactorMultiply ), 2, ctpg::associativity::ltor 
            );
        constexpr static const auto divide_term = ctpg::char_term( Warp::Utilities::to_char( 
                    ExpressionOperator::FactorDivide ), 2, ctpg::associativity::ltor 
            );


        constexpr static const auto left_parenthesis_term = ctpg::char_term( Warp::Utilities::to_char( 
                    ScopeOperators::OpenParenthesis ), 4, ctpg::associativity::ltor 
            );
        constexpr static const auto right_parenthesis_term = ctpg::char_term( Warp::Utilities::to_char( 
                    ScopeOperators::CloseParenthesis ), 4, ctpg::associativity::ltor 
            );

        constexpr static const auto and_term = ctpg::string_term( and_operator, 6, ctpg::associativity::ltor );
        constexpr static const auto or_term = ctpg::string_term( and_operator, 5, ctpg::associativity::ltor );
        constexpr static const auto not_term = ctpg::char_term( not_operator, 4, ctpg::associativity::ltor );

        template< auto ParameterConstant >
        using ResolvedType = typename TypeResolverParameterType< ParameterConstant >::Type;

        constexpr static const auto parser = ctpg::parser( 
                factor, 
                ctpg::terms( 
                        multiply_term, 
                        divide_term, 
                        plus_term, 
                        minus_term, 
                        term< RegexLiteralTerms::NaturalNumber >, 
                        left_parenthesis_term, 
                        right_parenthesis_term 
                    ), 
                ctpg::nterms( 
                        factor, 
                        sum, 
                        parenthesis_scope 
                    ), 
                ctpg::rules( 
                        factor( term< RegexLiteralTerms::NaturalNumber > ) 
                                >= []( auto token ) {
                                        return Warp::Utilities::allocate_integral_literal_node< 
                                                ResolvedType< RegexLiteralTerms::NaturalNumber > 
                                            >( token );
                                    }, 
                        factor( factor, multiply_term, term< RegexLiteralTerms::NaturalNumber > ) 
                                >= []( auto current_factor, auto, const auto& next_token )
                                    {
                                        return Warp::Utilities::allocate_node< Warp::Parser::ExpressionOperator::FactorMultiply >( 
                                                current_factor, 
                                                Warp::Utilities::allocate_integral_literal_node< 
                                                        ResolvedType< RegexLiteralTerms::NaturalNumber > 
                                                    >( next_token )
                                            );
                                    }, 
                        factor( factor, divide_term, term< RegexLiteralTerms::NaturalNumber > ) 
                                >= []( auto current_factor, auto, const auto& next_token )
                                    {
                                        return Warp::Utilities::allocate_node< ExpressionOperator::FactorDivide >( 
                                                current_factor, 
                                                Warp::Utilities::allocate_integral_literal_node< 
                                                        ResolvedType< RegexLiteralTerms::NaturalNumber > 
                                                    >( next_token )
                                            );
                                    }, 
                        parenthesis_scope( left_parenthesis_term, factor, right_parenthesis_term )
                                >= [] ( auto, auto factor, auto ) { return factor; }, 
                        factor( parenthesis_scope ) >= []( auto parenthesis_scope ) { return parenthesis_scope; }, 
                        factor( factor, multiply_term, parenthesis_scope ) 
                                >= []( auto factor, auto, auto parenthesis_scope ) 
                                    {
                                        return Warp::Utilities::allocate_node< ExpressionOperator::FactorMultiply >( 
                                                factor, 
                                                parenthesis_scope 
                                            );
                                    }, 
                        factor( factor, divide_term, parenthesis_scope ) 
                                >= []( auto factor, auto, auto parenthesis_scope ) 
                                    {
                                        return Warp::Utilities::allocate_node< ExpressionOperator::FactorDivide >( 
                                                factor, 
                                                parenthesis_scope 
                                            );
                                    }, 
                        factor( sum ) 
                                >= []( auto sum ) {
                                    return sum;
                                }, 
                        sum( factor, plus_term, factor ) 
                                >= []( auto current_sum, auto, const auto& next_token ) 
                                    {
                                        return Warp::Utilities::allocate_node< ExpressionOperator::SumAdd >( 
                                                current_sum, 
                                                next_token 
                                            );
                                    }, 
                        sum( factor, minus_term, factor ) 
                                >= []( auto current_sum, auto, const auto& next_token )
                                    {
                                        return Warp::Utilities::allocate_node< ExpressionOperator::SumSubtract >( 
                                                current_sum, 
                                                next_token 
                                            );
                                    }
                )
        );
    };
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP
