#include <Warp/Terms.hpp>
#include <ctpg.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP

namespace Warp::Parser
{
    template< template< auto > typename TypeResolverParameterType = DefaultTypes >
    struct WarpParser
    {
        using TermsType = typename TermBuilder< 
                void, 
                1, 
                ExpressionOperator::SumAdd, 
                ExpressionOperator::SumSubtract 
            >::Next< 2 >::TermsType< 
                    ExpressionOperator::FactorMultiply, 
                    ExpressionOperator::FactorDivide 
                >::Next< 3 >::TermsType< 
                        ScopeOperators::OpenParenthesis, 
                        ScopeOperators::CloseParenthesis 
                    >::Next< TermBuilderType::NoPriority >::TermsType< 
                            RegexLiteralTerms::Identifier, 
                            RegexLiteralTerms::NaturalNumber 
                        >;

        using NonTerminalTermsType = TermBuilder< 
                void, 
                TermBuilderType::NoPriority, 
                NonTerminalTerms::Factor, 
                NonTerminalTerms::Sum, 
                NonTerminalTerms::ParenthesisScope 
            >;

        constexpr static const auto terms = TermsType::to_tuple();
        constexpr static const auto non_terminal_terms = NonTerminalTermsType::to_tuple();

        template< auto ParameterConstant >
        constexpr const static auto term = TermsType::get_term< ParameterConstant >();

        template< auto ParameterConstant >
        constexpr const static auto non_terminal_term = NonTerminalTermsType::get_term< ParameterConstant >();

        template< auto ParameterConstant >
        using ResolvedType = typename TypeResolverParameterType< ParameterConstant >::Type;

        constexpr static const auto parser = ctpg::parser( 
                non_terminal_term< NonTerminalTerms::Factor >, 
                // ctpg::terms( 
                //         term< ExpressionOperator::FactorMultiply >, 
                //         term< ExpressionOperator::FactorDivide >, 
                //         term< ExpressionOperator::SumAdd >, 
                //         term< ExpressionOperator::SumSubtract >, 
                //         term< RegexLiteralTerms::NaturalNumber >, 
                //         term< RegexLiteralTerms::Identifier >, 
                //         term< ScopeOperators::OpenParenthesis >, 
                //         term< ScopeOperators::CloseParenthesis >  
                //     ), 
                terms, 
                // ctpg::nterms( 
                //         non_terminal_term< NonTerminalTerms::Factor >, 
                //         non_terminal_term< NonTerminalTerms::Sum >, 
                //         non_terminal_term< NonTerminalTerms::ParenthesisScope > //, 
                //         // function_alternative 
                //     ), 
                non_terminal_terms, 
                ctpg::rules( 
                        non_terminal_term< NonTerminalTerms::Factor >( term< RegexLiteralTerms::NaturalNumber > ) 
                                >= []( auto token ) {
                                        return Warp::Utilities::allocate_integral_literal_node< 
                                                ResolvedType< RegexLiteralTerms::NaturalNumber > 
                                            >( token );
                                    }, 
                        non_terminal_term< NonTerminalTerms::Factor >( 
                                    non_terminal_term< NonTerminalTerms::Factor >, 
                                    term< ExpressionOperator::FactorMultiply >, 
                                    term< RegexLiteralTerms::NaturalNumber > 
                                ) 
                            >= []( auto current_factor, auto, const auto& next_token )
                                    {
                                        return Warp::Utilities::allocate_node< ExpressionOperator::FactorMultiply >( 
                                                current_factor, 
                                                Warp::Utilities::allocate_integral_literal_node< 
                                                        ResolvedType< RegexLiteralTerms::NaturalNumber > 
                                                    >( next_token )
                                            );
                                    }, 
                        non_terminal_term< NonTerminalTerms::Factor >( 
                                    non_terminal_term< NonTerminalTerms::Factor >, 
                                    term< ExpressionOperator::FactorDivide >, 
                                    term< RegexLiteralTerms::NaturalNumber > 
                                ) 
                            >= []( auto current_factor, auto, const auto& next_token )
                                    {
                                        return Warp::Utilities::allocate_node< ExpressionOperator::FactorDivide >( 
                                                current_factor, 
                                                Warp::Utilities::allocate_integral_literal_node< 
                                                        ResolvedType< RegexLiteralTerms::NaturalNumber > 
                                                    >( next_token )
                                            );
                                    }, 
                        non_terminal_term< NonTerminalTerms::ParenthesisScope >( 
                                    term< ScopeOperators::OpenParenthesis >, 
                                    non_terminal_term< NonTerminalTerms::Factor >, 
                                    term< ScopeOperators::CloseParenthesis > 
                                ) 
                            >= [] ( auto, non_terminal_term< NonTerminalTerms::Factor >, auto ) { 
                                        return non_terminal_term< NonTerminalTerms::Factor >; 
                                    }, 
                        non_terminal_term< NonTerminalTerms::Factor >( 
                                    non_terminal_term< NonTerminalTerms::ParenthesisScope > 
                                ) 
                            >= []( non_terminal_term< NonTerminalTerms::ParenthesisScope > ) { 
                                        return non_terminal_term< NonTerminalTerms::ParenthesisScope >; 
                                    }, 
                        non_terminal_term< NonTerminalTerms::Factor >( 
                                    non_terminal_term< NonTerminalTerms::Factor >, 
                                    term< ExpressionOperator::FactorMultiply >, 
                                    non_terminal_term< NonTerminalTerms::ParenthesisScope > 
                                ) 
                            >= []( auto non_terminal_term< NonTerminalTerms::Factor >, auto, auto non_terminal_term< NonTerminalTerms::ParenthesisScope > ) 
                                    {
                                        return Warp::Utilities::allocate_node< ExpressionOperator::FactorMultiply >( 
                                                non_terminal_term< NonTerminalTerms::Factor >, 
                                                non_terminal_term< NonTerminalTerms::ParenthesisScope > 
                                            );
                                    }, 
                        non_terminal_term< NonTerminalTerms::Factor >( 
                                        non_terminal_term< NonTerminalTerms::Factor >, 
                                        term< ExpressionOperator::FactorDivide >, 
                                        non_terminal_term< NonTerminalTerms::ParenthesisScope > 
                                    ) 
                                >= []( auto non_terminal_term< NonTerminalTerms::Factor >, auto, auto non_terminal_term< NonTerminalTerms::ParenthesisScope > ) 
                                    {
                                        return Warp::Utilities::allocate_node< ExpressionOperator::FactorDivide >( 
                                                non_terminal_term< NonTerminalTerms::Factor >, 
                                                non_terminal_term< NonTerminalTerms::ParenthesisScope > 
                                            );
                                    }, 
                        non_terminal_term< NonTerminalTerms::Factor >( non_terminal_term< NonTerminalTerms::Sum > ) 
                                >= []( auto sum > ) {
                                        return non_terminal_term< NonTerminalTerms::Sum >;
                                    }, 
                        non_terminal_term< NonTerminalTerms::Sum >( 
                                        non_terminal_term< NonTerminalTerms::Factor >, 
                                        term< ExpressionOperator::SumAdd >, 
                                        non_terminal_term< NonTerminalTerms::Factor > 
                                    ) 
                                >= []( auto current_sum, auto, const auto& next_token ) 
                                    {
                                        return Warp::Utilities::allocate_node< ExpressionOperator::SumAdd >( 
                                                current_sum, 
                                                next_token 
                                            );
                                    }, 
                        non_terminal_term< NonTerminalTerms::Sum >( 
                                        non_terminal_term< NonTerminalTerms::Factor >, 
                                        term< ExpressionOperator::SumSubtract >, 
                                        non_terminal_term< NonTerminalTerms::Factor > 
                                    ) 
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
