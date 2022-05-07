#include <Warp/Terms.hpp>
#include <ctpg.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP

namespace Warp::Parser
{
    template< template< auto > typename TypeResolverParameterType = DefaultTypes >
    struct WarpParser
    {
        using enum ExpressionOperator;
        using enum ScopeOperators;
        using enum RegexLiteralTerms;
        using enum StringTerms;
        using enum NonTerminalTerms;
        using enum BooleanOperator;

        using TermsType = EasySafeTermsType< 
                SumAdd, 
                SumSubtract 
            >::AddOnePriority< 
                    FactorMultiply, 
                    FactorDivide 
                // >::AddOnePriority< 
                //         BiCondition, 
                //         Implies 
                //     >::AddOnePriority< 
                //             Or 
                //         >::AddOnePriority< 
                //                 And 
                        >::AddOnePriority<  
                                BooleanOperator::LogicalNot 
                            >::AddOnePriority< 
                                    OpenParenthesis, 
                                    CloseParenthesis 
                                >::AddOnePriority<
                                        Identifier 
                                    // >::AddOnePriority< 
                                        >::NoPriority< 
                                            BooleanLiteral, 
                                                NaturalNumber 
                                            >;

        using NonTerminalTermsType = SafeTermsType< 
                TermBuilderType::NoPriority, 
                // Non Terminal Terms Begin Here, they have no //
                // "priority" with respect to other operators  //
                Factor, 
                Sum, 
                ParenthesisScope, 
                LogicalOperation 
            >;

        template< auto ParameterConstant >
        constexpr const static auto term = TermsType::get_term< ParameterConstant >();

        template< auto ParameterConstant >
        constexpr const static auto non_terminal_term = NonTerminalTermsType::get_term< ParameterConstant >();

        constexpr static const auto terms = TermsType::to_tuple();
        constexpr static const auto non_terminal_terms = NonTerminalTermsType::to_tuple();

        template< auto ParameterConstant >
        using ResolvedType = typename TypeResolverParameterType< ParameterConstant >::Type;

        constexpr static const auto parser = ctpg::parser( 
                non_terminal_term< LogicalOperation >, 
                terms, 
                non_terminal_terms, 
                ctpg::rules( 

                        // CTPG Makes it very hard to seperate all these out (see: https://github.com/peter-winter/ctpg/issues/46 ) //
                        // I spent a good amount of time doings so I am going to annotate the different sections. ////////////////////

                        //////////////////////////////// Arithmatic Expressions ////////////////////////////////

                        non_terminal_term< Factor >( term< NaturalNumber > ) 
                                >= []( auto token ) {
                                        return Warp::Utilities::allocate_integral_literal_node< 
                                                ResolvedType< NaturalNumber > 
                                            >( token );
                                    }, 
                        non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorMultiply >, term< NaturalNumber > ) 
                                >= []( auto current_factor, auto, const auto& next_token )
                                    {
                                        return Warp::Utilities::allocate_node< FactorMultiply >( 
                                                current_factor, 
                                                Warp::Utilities::allocate_integral_literal_node< 
                                                        ResolvedType< NaturalNumber > 
                                                    >( next_token )
                                            );
                                    }, 
                        non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorDivide >, term< NaturalNumber > ) 
                                >= []( auto current_factor, auto, const auto& next_token )
                                    {
                                        return Warp::Utilities::allocate_node< FactorDivide >( 
                                                current_factor, 
                                                Warp::Utilities::allocate_integral_literal_node< 
                                                        ResolvedType< NaturalNumber > 
                                                    >( next_token )
                                            );
                                    }, 
                        non_terminal_term< ParenthesisScope >( term< OpenParenthesis >, non_terminal_term< Factor >, term< CloseParenthesis > )
                                >= [] ( auto, auto factor, auto ) { return factor; }, 
                        non_terminal_term< Factor >( non_terminal_term< ParenthesisScope > ) 
                                >= []( auto parenthesis_scope ) { return parenthesis_scope; }, 
                        non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorMultiply >, non_terminal_term< ParenthesisScope > ) 
                                >= []( auto factor, auto, auto parenthesis_scope ) 
                                    {
                                        return Warp::Utilities::allocate_node< FactorMultiply >( 
                                                factor, 
                                                parenthesis_scope 
                                            );
                                    }, 
                        non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorDivide >, non_terminal_term< ParenthesisScope > ) 
                                >= []( auto factor, auto, auto parenthesis_scope ) 
                                    {
                                        return Warp::Utilities::allocate_node< FactorDivide >( 
                                                factor, 
                                                parenthesis_scope 
                                            );
                                    }, 
                        non_terminal_term< Factor >( non_terminal_term< Sum > ) 
                                >= []( auto sum ) {
                                    return sum;
                                }, 
                        non_terminal_term< Sum >( non_terminal_term< Factor >, term< SumAdd >, non_terminal_term< Factor > ) 
                                >= []( auto current_sum, auto, const auto& next_token ) 
                                    {
                                        return Warp::Utilities::allocate_node< SumAdd >( 
                                                current_sum, 
                                                next_token 
                                            );
                                    }, 
                        non_terminal_term< Sum >( non_terminal_term< Factor >, term< SumSubtract >, non_terminal_term< Factor > ) 
                                >= []( auto current_sum, auto, const auto& next_token )
                                    {
                                        return Warp::Utilities::allocate_node< SumSubtract >( 
                                                current_sum, 
                                                next_token 
                                            );
                                    }, 
                        
                        //////////////////////////////// Boolean Expressions ////////////////////////////////

                        // Probably shouldent actually ever need thes in the language, but keeping them at least for debugging. //
                        non_terminal_term< LogicalOperation >( term< BooleanLiteral > ) 
                                >= []( auto token ) {
                                    return Warp::Utilities::allocate_boolean_literal_node( token );
                                }, 
                        non_terminal_term< LogicalOperation >( term< BooleanOperator::LogicalNot >, non_terminal_term< LogicalOperation > )
                                >= []( auto, auto logical_expression ) {
                                    Warp::Utilities::allocate_node< BooleanOperator::LogicalNot >( logical_expression );
                                }
                )
        );
    };
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP
