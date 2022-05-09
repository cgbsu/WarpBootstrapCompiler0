#include <Warp/Terms.hpp>
#include <ctpg.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP

namespace Warp::Parser
{
    #define WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON( COMPARISON_TYPE ) \
        non_terminal_term< Comparison >( non_terminal_term< Factor >, term< COMPARISON_TYPE >, non_terminal_term< Factor > ) \
            >= []( auto left, auto, auto right ) { \
                return Warp::Utilities::allocate_node< COMPARISON_TYPE >( left, right ); \
            }
    
    #define WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON_UNALIGNED( COMPARISON_TYPE, NODE_COMPARISON_TYPE ) \
        non_terminal_term< Comparison >( non_terminal_term< Factor >, term< COMPARISON_TYPE >, non_terminal_term< Factor > ) \
            >= []( auto left, auto, auto right ) { \
                return Warp::Utilities::allocate_node< NODE_COMPARISON_TYPE >( left, right ); \
            }

    #define WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( LEFT, OPERATION, RIGHT ) \
                        non_terminal_term< Boolean##OPERATION >( non_terminal_term< LEFT >, term< OPERATION >, non_terminal_term< RIGHT > ) \
                                >= []( auto right, auto, auto left ) \
                                { \
                                    return std::move( Warp::Utilities::allocate_node< Logical##OPERATION >( \
                                            left, \
                                            right \
                                        ) ); \
                                } 

    #define WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION( LEFT, OPERATION, RIGHT ) \
                        non_terminal_term< LogicalOperation >( non_terminal_term< LEFT >, term< OPERATION >, non_terminal_term< RIGHT > ) \
                                >= []( auto right, auto, auto left ) \
                                { \
                                    return std::move( Warp::Utilities::allocate_node< Logical##OPERATION >( \
                                            left, \
                                            right \
                                        ) ); \
                                } 

    template< template< auto > typename TypeResolverParameterType = DefaultTypes >
    struct WarpParser
    {
        using enum ExpressionOperator;
        using enum ScopeOperators;
        using enum RegexLiteralTerms;
        using enum StringTerms;
        using enum NonTerminalTerms;
        using enum BooleanOperator;
        using enum ComparisonOperator;

        using TermsType = EasySafeTermsType< 
                SumAdd, 
                SumSubtract 
            >::AddOnePriority< 
                    FactorMultiply, 
                    FactorDivide 
                >::AddOnePriority< 
                        BiCondition, 
                        Implies 
                    >::AddOnePriority< 
                            Or 
                        >::AddOnePriority< 
                                And 
                            >::AddOnePriority<  
                                    LogicalNot 
                                >::AddOnePriority< 
                                        ComparisonEqual, 
                                        ComparisonLessThan, 
                                        ComparisonGreaterThan, 
                                        GreaterThanOrEqualTo, 
                                        LessThanOrEqualTo 
                                    >::AddOnePriority< 
                                            OpenParenthesis, 
                                            CloseParenthesis 
                                        >::AddOnePriority<
                                                Identifier 
                                            >::NoPriority< 
                                                    BooleanLiteral, 
                                                    NaturalNumber 
                                                >; // I feel like Im writing python here 0.0 //

        using NonTerminalTermsType = SafeTermsType< 
                TermBuilderType::NoPriority, 
                // Non Terminal Terms Begin Here, they have no //
                // "priority" with respect to other operators  //
                Factor, 
                Sum, 
                ParenthesisScope, 
                LogicalOperation, 
                BooleanAnd, 
                BooleanOr, 
                Comparison 
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
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( LogicalOperation, And, LogicalOperation ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanAnd, And, LogicalOperation ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( LogicalOperation, Or, LogicalOperation ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanOr, Or, LogicalOperation ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanAnd, Or, LogicalOperation ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( LogicalOperation, Or, BooleanAnd ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanAnd, Or, BooleanAnd ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION( LogicalOperation, BiCondition, LogicalOperation ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION( LogicalOperation, Implies, LogicalOperation ), 

                        non_terminal_term< LogicalOperation >( non_terminal_term< BooleanOr > )
                                >=[] ( auto or_expression ) {
                                    return or_expression;
                                }, 
                        non_terminal_term< LogicalOperation >( non_terminal_term< BooleanAnd > )
                                >=[] ( auto and_expression ) {
                                    return and_expression;
                                }, 
                        
                        non_terminal_term< LogicalOperation >( term< LogicalNot >, non_terminal_term< LogicalOperation > )
                                >= []( auto, auto logical_expression ) {
                                    return Warp::Utilities::allocate_node< LogicalNot >( logical_expression );
                                }, 
                        non_terminal_term< LogicalOperation >( term< LogicalNot >, term< LogicalNot >, non_terminal_term< LogicalOperation > )
                                >= []( auto, auto, auto logical_expression ) {
                                    return logical_expression;
                                }, 
                        non_terminal_term< LogicalOperation >( term< OpenParenthesis >, non_terminal_term< LogicalOperation >, term< CloseParenthesis > )
                                >= []( auto, auto logical_expression, auto ) {
                                    return logical_expression;
                                }, 
                        non_terminal_term< LogicalOperation >( term< OpenParenthesis >, non_terminal_term< BooleanAnd >, term< CloseParenthesis > )
                                >= []( auto, auto logical_expression, auto ) {
                                    return logical_expression;
                                }, 
                        non_terminal_term< LogicalOperation >( term< OpenParenthesis >, non_terminal_term< BooleanOr >, term< CloseParenthesis > )
                                >= []( auto, auto logical_expression, auto ) {
                                    return logical_expression;
                                }, 


                        //////////////////////////////// Comparison Expressions ////////////////////////////////

                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON( ComparisonEqual ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON( ComparisonLessThan ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON( ComparisonGreaterThan ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON_UNALIGNED( GreaterThanOrEqualTo, ComparisionGreaterThanOrEqualTo ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON_UNALIGNED( LessThanOrEqualTo, ComparisionLessThanOrEqualTo ) 
                )
        );
    };
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP
