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

    
    #define WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_REDUCE_MIDDLE_OF_PARENTHESIS_TO( TO, MIDDLE ) \
        non_terminal_term< TO >( term< OpenParenthesis >, non_terminal_term< MIDDLE >, term< CloseParenthesis > ) \
            >= []( auto, auto middle, auto ) { \
                return middle; \
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
        using enum FunctionOperators;

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
                                                FunctionParameterConstaraint 
                                            >::AddOnePriority< 
                                                    Comma 
                                                >::AddOnePriority<
                                                        Identifier 
                                                    // >::AddOnePriority< 
                                                            // KeywordLet 
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
                Comparison, 
                // Parameter, 
                ParameterList//, 
                // FunctionAlternative 
            >;

        template< auto ParameterConstant >
        constexpr const static auto term = TermsType::get_term< ParameterConstant >();

        template< auto ParameterConstant >
        constexpr const static auto non_terminal_term = NonTerminalTermsType::get_term< ParameterConstant >();

        constexpr static const auto terms = TermsType::to_tuple();
        constexpr static const auto non_terminal_terms = NonTerminalTermsType::to_tuple();

        template< auto ParameterConstant >
        using ResolvedType = typename TypeResolverParameterType< ParameterConstant >::Type;

        // constexpr std::vector< Warp::CompilerRuntime::Parameter >&& add_to_parameter_list( const auto& list, const  )

        constexpr static const auto parser = ctpg::parser( 
                non_terminal_term< ParameterList >, 
                terms, 
                non_terminal_terms, 
                ctpg::rules( 

                        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        // CTPG Makes it very hard to seperate all these out (see: https://github.com/peter-winter/ctpg/issues/46 ) //
                        // I spent a good amount of time doings so I am going to annotate the different sections. I also have not ////
                        // yet figured out how to generate rules externally and automatically, so please excuese the macros and //////
                        // crummy code overall here, I will just have to document it a bit. //////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////////////////////////////////////////

                        //////////////////////////////// Arithmatic Expressions ////////////////////////////////

                        non_terminal_term< Factor >( term< Identifier > ) 
                                >= []( auto token ) {
                                        return std::move( Warp::Utilities::allocate_node< Warp::AbstractSyntaxTree::NodeType::Identifier >( Warp::Utilities::hash_string( token ) ) );
                                }, 
                        non_terminal_term< Factor >( term< NaturalNumber > ) 
                                >= []( auto token )
                                {
                                        return std::move( Warp::Utilities::allocate_integral_literal_node< 
                                                ResolvedType< NaturalNumber > 
                                            >( token ) );
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
                        
                        // And::Initial_Conditions //
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( LogicalOperation, And, LogicalOperation ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( LogicalOperation, And, Comparison ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( Comparison, And, Comparison ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( Comparison, And, LogicalOperation ), 
                        
                        // And::Chaining //
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanAnd, And, LogicalOperation ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanAnd, And, Comparison ), 

                        // Or::Initial_Conditions //
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( LogicalOperation, Or, LogicalOperation ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( Comparison, Or, LogicalOperation ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( Comparison, Or, Comparison ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( LogicalOperation, Or, Comparison ), 

                        // Or::Chaining::And //
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanAnd, Or, BooleanAnd ), 

                        // Or::Chaining::LogicalOperation //
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanOr, Or, LogicalOperation ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanAnd, Or, LogicalOperation ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( LogicalOperation, Or, BooleanAnd ), 

                        // Or::Chaning::Comparison //
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanOr, Or, Comparison ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanAnd, Or, Comparison ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( Comparison, Or, BooleanAnd ), 
                        
                        //Extended Logical (evalated last before aritmatic) //
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION( LogicalOperation, BiCondition, LogicalOperation ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION( LogicalOperation, Implies, LogicalOperation ), 

                        // Not //
                        non_terminal_term< LogicalOperation >( term< LogicalNot >, non_terminal_term< LogicalOperation > )
                                >= []( auto, auto logical_expression ) {
                                    return Warp::Utilities::allocate_node< LogicalNot >( logical_expression );
                                }, 
                        // This seems to do the trick with reducing the not's just down to one or two depending on if there are an even or odd amount. //
                        non_terminal_term< LogicalOperation >( term< LogicalNot >, term< LogicalNot >, non_terminal_term< LogicalOperation > )
                                >= []( auto, auto, auto logical_expression ) {
                                    return logical_expression;
                                }, 
                        
                        // No other logical terms here, just return what you've got. //
                        non_terminal_term< LogicalOperation >( non_terminal_term< BooleanOr > )
                                >= []( auto or_expression ) {
                                    return or_expression;
                                }, 
                        non_terminal_term< LogicalOperation >( non_terminal_term< BooleanAnd > )
                                >= []( auto and_expression ) {
                                    return and_expression;
                                },                         
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_REDUCE_MIDDLE_OF_PARENTHESIS_TO( LogicalOperation, LogicalOperation ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_REDUCE_MIDDLE_OF_PARENTHESIS_TO( LogicalOperation, BooleanAnd ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_REDUCE_MIDDLE_OF_PARENTHESIS_TO( LogicalOperation, BooleanOr ), 

                        //////////////////////////////// Comparison Expressions ////////////////////////////////

                        // Comparisions must return a Comparison non terminal term so they can be evaluated before the logical expressions (and thus used in them) //

                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON( ComparisonEqual ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON( ComparisonLessThan ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON( ComparisonGreaterThan ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON_UNALIGNED( GreaterThanOrEqualTo, ComparisionGreaterThanOrEqualTo ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON_UNALIGNED( LessThanOrEqualTo, ComparisionLessThanOrEqualTo ), 
                        WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_REDUCE_MIDDLE_OF_PARENTHESIS_TO( LogicalOperation, Comparison ), 

                        // TODO: Add ternary/chaining comparisons and : "piping/forwarding" //

                        //////////////////////////////// Functions::Parameters ////////////////////////////////

                        // TODO: Add types/concepts with additional color, e.g my_parameter : natural : my_parameter < 128

                        non_terminal_term< ParameterList >( term< Identifier >, term< FunctionParameterConstaraint >, non_terminal_term< Comparison > )
                                >= []( auto parameter_name, auto, auto comparison_constraint )
                                {
                                        std::vector< Warp::CompilerRuntime::Parameter > parameter_list;
                                        parameter_list.push_back( Warp::CompilerRuntime::Parameter{ 
                                                Warp::Utilities::hash_string( parameter_name ), 
                                                comparison_constraint 
                                            } );
                                        return std::move( parameter_list );
                                }, 
                        non_terminal_term< ParameterList >( term< Identifier >, term< FunctionParameterConstaraint >, non_terminal_term< LogicalOperation > )
                                >= []( auto parameter_name, auto, auto comparison_constraints )
                                {
                                        std::vector< Warp::CompilerRuntime::Parameter > parameter_list;
                                        parameter_list.push_back( Warp::CompilerRuntime::Parameter{ 
                                                Warp::Utilities::hash_string( parameter_name ), 
                                                comparison_constraints 
                                            } );
                                        return std::move( parameter_list );
                                },
                        non_terminal_term< ParameterList >( non_terminal_term< ParameterList >, term< Comma >, term< Identifier >, term< FunctionParameterConstaraint >, non_terminal_term< Comparison > )
                                >= []( auto parameter_list, auto, auto parameter_name, auto, auto comparison_constraint )
                                {
                                        parameter_list.push_back( Warp::CompilerRuntime::Parameter{ 
                                                Warp::Utilities::hash_string( parameter_name ), 
                                                comparison_constraint 
                                            } );
                                        return std::move( parameter_list );
                                }, 
                        non_terminal_term< ParameterList >( non_terminal_term< ParameterList >, term< Comma >, term< Identifier >, term< FunctionParameterConstaraint >, non_terminal_term< LogicalOperation > )
                                >= []( auto parameter_list, auto, auto parameter_name, auto, auto comparison_constraints )
                                {
                                        parameter_list.push_back( Warp::CompilerRuntime::Parameter{ 
                                                Warp::Utilities::hash_string( parameter_name ), 
                                                comparison_constraints 
                                            } );
                                        return std::move( parameter_list );
                                }
                        //////////////////////////////// Functions (The final boss) ////////////////////////////////                        

                        // non_terminal_term< Parameter >( term< Identifier >, non_terminal_term< LogicalOperation > )

                )
        );
    };
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP
