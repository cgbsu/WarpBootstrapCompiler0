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
            // >::AddOnePriority< 
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
                                                    FunctionParameterNextParameter, 
                                                                    hash_symbol
                                                >::AddOnePriority<
                                                        Identifier 
                                                    >::AddOnePriority< 
                                                            FunctionDefintionComplete 
                                                        >::AddOnePriority< 
                                                                KeywordLet 
                                                            >::NoPriority< 
                                                                    BooleanLiteral, 
                                                                    NaturalNumber, 
                                                                    FunctionDefinitionOperator, 
                                                                    FunctionResult//, 
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
                ParameterList, 
                WarpFunctionAlternative, 
                WarpFunction, 
                WarpModule, 
                Arguments, 
                Expression, 
                ExpressionEater, 
                Call, 
                CallNode, 
                CallArguments, 
                CallEater 
            >;

        template< auto ParameterConstant >
        constexpr const static auto term = TermsType::get_term< ParameterConstant >();

        template< auto ParameterConstant >
        constexpr const static auto non_terminal_term = NonTerminalTermsType::get_term< ParameterConstant >();

        constexpr static const auto terms = TermsType::to_tuple();
        constexpr static const auto non_terminal_terms = NonTerminalTermsType::to_tuple();

        template< auto ParameterConstant >
        using ResolvedType = typename TypeResolverParameterType< ParameterConstant >::Type;

        constexpr static auto add_to_parameter_list( 
                auto& parameter_list, 
                auto parameter_name, 
                auto constraints 
            ) 
        {
                parameter_list.input_constraints.push_back( Warp::CompilerRuntime::Parameter{ 
                        Warp::Utilities::hash_string( parameter_name ), 
                        constraints 
                    } );
                return std::move( parameter_list );
        }

        template< auto NodeTagParameterConstant >
        constexpr static auto subsume_expression( auto left, auto right ) {
            return Warp::Utilities::allocate_node< NodeTagParameterConstant >( 
                    left, 
                    right 
                );
        }

        template< auto NodeTagParameterConstant >
        constexpr static auto subsume_function_alternative_expression( auto function, auto factor )
        {
            return Warp::CompilerRuntime::FunctionAlternative{ 
                    function.identifier, 
                    subsume_expression< NodeTagParameterConstant >( 
                        function.expression, 
                        factor 
                    ), 
                    function.input_constraints 
                };
        }

        constexpr static const auto parser = ctpg::parser( 
                non_terminal_term< Factor >, 
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

                        non_terminal_term< Factor >( term< FunctionResult > ) 
                                >= []( auto token ) {
                                        return std::move( Warp::Utilities::allocate_node< FunctionResult >() );
                                }, 
                        non_terminal_term< Factor >( term< Identifier > ) 
                                >= []( auto token ) {
                                        return std::move( Warp::Utilities::allocate_node< 
                                                        Warp::AbstractSyntaxTree::NodeType::Identifier 
                                                >( Warp::Utilities::hash_string( token ) ) );
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
                        non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorMultiply >, term< Identifier > ) 
                                >= []( auto current_factor, auto, const auto& next_token )
                                    {
                                        return Warp::Utilities::allocate_node< FactorMultiply >( 
                                                current_factor, 
                                                Warp::Utilities::allocate_node< 
                                                        Warp::AbstractSyntaxTree::NodeType::Identifier 
                                                    >( Warp::Utilities::hash_string( next_token ) )
                                            );
                                    }, 
                        non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorDivide >, term< Identifier > ) 
                                >= []( auto current_factor, auto, const auto& next_token )
                                    {
                                        return Warp::Utilities::allocate_node< FactorDivide >( 
                                                current_factor, 
                                                Warp::Utilities::allocate_node< 
                                                        Warp::AbstractSyntaxTree::NodeType::Identifier 
                                                    >( Warp::Utilities::hash_string( next_token ) )
                                            );
                                    }, 

                        // non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorMultiply >, non_terminal_term< CallNode > ) 
                        //         >= []( auto current_factor, auto, auto call )
                        //             {
                        //                 return Warp::Utilities::allocate_node< FactorMultiply >( 
                        //                         current_factor, 
                        //                         call
                        //                     );
                        //             }, 
                        // non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorDivide >, non_terminal_term< CallNode > ) 
                        //         >= []( auto current_factor, auto, auto call )
                        //             {
                        //                 return Warp::Utilities::allocate_node< FactorDivide >( 
                        //                         current_factor, 
                        //                         call 
                        //                     );
                        //             }, 


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
                        non_terminal_term< Expression >( non_terminal_term< Factor >) 
                                >= []( auto factor ) {
                                    return factor;
                                }, 
                        // non_terminal_term< Expression >( term< FunctionDefinitionOperator >, non_terminal_term< Factor > ) 
                        //         >= []( auto , auto factor ) {
                        //             return factor;
                        //         }, 

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

                        non_terminal_term< ParameterList >( non_terminal_term< ParameterList >, term< Identifier >, term< FunctionParameterConstaraint >, non_terminal_term< Comparison > )
                                >= []( auto parameter_list, auto parameter_name, auto, auto comparison_constraint ) {
                                        return add_to_parameter_list( parameter_list, parameter_name, comparison_constraint );
                                }, 
                        non_terminal_term< ParameterList >( non_terminal_term< ParameterList >, term< Identifier >, term< FunctionParameterConstaraint >, non_terminal_term< LogicalOperation > )
                                >= []( auto parameter_list, auto parameter_name, auto, auto constraints ) {
                                        return add_to_parameter_list( parameter_list, parameter_name, constraints );
                                },
                        non_terminal_term< ParameterList >( non_terminal_term< ParameterList >, term< FunctionParameterNextParameter > )
                                >= []( auto parameter_list, auto ) {
                                    return parameter_list;
                                }, 
                        non_terminal_term< ParameterList >( non_terminal_term< ParameterList >, term< FunctionParameterNextParameter > )
                                >= []( auto parameter_list, auto ) {
                                    return parameter_list;
                                }, 
                        non_terminal_term< ParameterList >( term< KeywordLet >, term< Identifier >, term< OpenParenthesis > )
                                >= []( auto let, auto function_name, auto open_parenthesis ) {
                                    return Warp::CompilerRuntime::IntermediateFunctionAlternative{ Warp::Utilities::hash_string( function_name ) };
                                },
                        non_terminal_term< Arguments >( non_terminal_term< ParameterList >, term< CloseParenthesis > )
                                >= []( auto parameter_list, auto ) {
                                    return parameter_list;
                                }, 
                        non_terminal_term< ExpressionEater >( non_terminal_term< Arguments >, term< FunctionDefinitionOperator >, non_terminal_term< Expression > )
                                >= []( auto arguments, auto, auto expression )
                                {
                                    std::cout << "Creating Expression Eater\n";
                                    return Warp::CompilerRuntime::FunctionAlternative{ 
                                        arguments.identifier, 
                                        expression, 
                                        // Warp::Utilities::allocate_node< Warp::AbstractSyntaxTree::NodeType::Unconstrained >(), 
                                        arguments.input_constraints, 
                                        // dependancies would go here //
                                    };
                                }, 
                        
                        //////////////////////////////// Arithmatic Expressions (Again) ////////////////////////////////

                        ///////////////////////////////////////////////////////////////////////////////////////////////
                        // This file is already a horrid mess, however, I needed to do duplicate this because I have //
                        // the prototype of the function to the left and the uparsed expression too the right which ///
                        // needs to be reduced before it can become a part of the function. ///////////////////////////
                        ///////////////////////////////////////////////////////////////////////////////////////////////

                        non_terminal_term< ExpressionEater >( non_terminal_term< ExpressionEater >, term< FactorMultiply >, non_terminal_term< Expression > ) 
                                >= []( auto function, auto, auto factor ) {
                                        return subsume_function_alternative_expression< FactorMultiply >( function, factor );
                                    },                         
                        non_terminal_term< ExpressionEater >( non_terminal_term< ExpressionEater >, term< FactorDivide >, non_terminal_term< Expression > ) 
                                >= []( auto function, auto, auto factor ) {
                                        return subsume_function_alternative_expression< FactorDivide >( function, factor );
                                    }, 




                        non_terminal_term< WarpFunctionAlternative >( non_terminal_term< ExpressionEater >, term< FunctionDefintionComplete > ) 
                                >= []( auto function, auto ) {
                                        return function;
                                    }, 
                        
                        //////////////////////////////// Functions::Calls ////////////////////////////////

                        non_terminal_term< Call >( term< Identifier >, term< OpenParenthesis > ) 
                                >= []( auto identifier, auto ) // function, auto identifier, auto )
                                    {
                                        std::cout << "Call found!\n";
                                        // return CallEaterCarrier{ 
                                                // function,
                                                return  
                                                Warp::CompilerRuntime::CallType{ 
                                                        Warp::Utilities::hash_string( identifier ), 
                                                        Warp::Utilities::VectorType< Warp::AbstractSyntaxTree::NodeVariantType >{} 
                                                    }
                                                    ;
                                            // };
                                    }, 
                        non_terminal_term< Call >( non_terminal_term< Call >, non_terminal_term< Factor >, term< FunctionParameterNextParameter > ) 
                                >= [](auto call, auto argument, auto )
                                    {
                                        std::cout << "Argument buffered!\n";
                                        // return CallEaterCarrier{ 
                                                // function,
                                                return  
                                                Warp::CompilerRuntime::CallType{ 
                                                        call.identifier, 
                                                        Warp::Utilities::VectorType< Warp::AbstractSyntaxTree::NodeVariantType >{ 
                                                                call.arguments, 
                                                                argument 
                                                            } 
                                                    }
                                                    ;
                                            // };
                                    }, 
                        non_terminal_term< Factor >( non_terminal_term< Call >, non_terminal_term< Factor >, term< CloseParenthesis > ) 
                                >= []( auto call, auto argument, auto )
                                    {
                                        std::cout << "Call end!\n";
                                        // return Warp::CompilerRuntime::FunctionAlternative{ 
                                                // call.function_alternative.identifier, 
                                                return Warp::Utilities::allocate_node< Warp::AbstractSyntaxTree::NodeType::FunctionCall >( 
                                                    call.identifier, 
                                                    Warp::Utilities::VectorType< Warp::AbstractSyntaxTree::NodeVariantType >{ 
                                                            call.arguments, 
                                                            argument 
                                                        } 
                                                );//, 
                                                // call.function_alternative.input_constraints 
                                            // };

                                        // return subsume_function_alternative_expression< Warp::AbstractSyntaxTree::NodeType::FunctionCall >( 
                                        //             call.function_alternative, 
                                        //             Warp::Utilities::allocate_node< Warp:: call.call 
                                        //         );
                                    }
                        // non_terminal_term< CallEater >( non_terminal_term< ExpressionEater >, term< Identifier >, term< OpenParenthesis > ) 
                        //         >= []( auto function, auto call, auto ) {
                        //                 std::cout << "Call found!\n";
                        //                 return CallEaterCarrier{ 
                        //                         function, 
                        //                         Warp::CompilerRuntime::CallType{ 
                        //                                 Warp::Utilities::hash_string( identifier ), 
                        //                                 Warp::Utilities::VectorType< Warp::AbstractSyntaxTree::NodeVariantType >{} 
                        //                             }
                        //                     };
                        //             }, 

///////////////////////////////////
                        // non_terminal_term< Call >( term< hash_symbol > )
                        //         >= []( auto identifier )
                        //             {
                        //                 std::cout << "Call found!\n";
                        //                 return Warp::CompilerRuntime::CallType{ 
                        //                         0, 
                        //                         // Warp::Utilities::get_node_value< Warp::AbstractSyntaxTree::NodeType::Identifier, Warp::Utilities::HashedStringType >( identifier ), 
                        //                         Warp::Utilities::VectorType< Warp::AbstractSyntaxTree::NodeVariantType >{} 
                        //                     };
                        //             }, 
                        // non_terminal_term< Call >( term< Identifier >, term< OpenParenthesis > )
                        //         >= []( auto identifier, auto )
                        //             {
                        //             }, 
                        // // non_terminal_term< Call >( non_terminal_term< Call >, non_terminal_term< Factor > )
                        // //         >= []( auto call, auto factor ) {
                        // //                 return Warp::CompilerRuntime::CallType{ 
                        // //                         call.identifier, 
                        // //                         Warp::Utilities::VectorType< Warp::AbstractSyntaxTree::NodeVariantType >{ call.arguments, factor } 
                        // //                     };
                        // //             }, 

                        // // non_terminal_term< Call >( non_terminal_term< Call >, term< FunctionParameterNextParameter >, non_terminal_term< CallNode > )
                        // //         >= []( auto call, auto, auto factor )
                        // //             {
                        // //                 std::cout << "New Parmaeter found!\n";
                        // //                 return Warp::CompilerRuntime::CallType{ 
                        // //                         call.identifier, 
                        // //                         Warp::Utilities::VectorType< Warp::AbstractSyntaxTree::NodeVariantType >{ call.arguments, factor } 
                        // //                     };
                        // //             }, 

                        // non_terminal_term< Call >( non_terminal_term< Call >, term< FunctionParameterNextParameter >, non_terminal_term< Factor > )
                        //         >= []( auto call, auto, auto factor )
                        //         {
                        //                 std::cout << "New Parmaeter found!\n";
                        //                 return Warp::CompilerRuntime::CallType{ 
                        //                         call.identifier, 
                        //                         Warp::Utilities::VectorType< Warp::AbstractSyntaxTree::NodeVariantType >{ call.arguments, factor } 
                        //                     };
                        //             }, 
                        // // non_terminal_term< Call >( non_terminal_term< Call >, term< FactorMultiply >, non_terminal_term< Factor > )
                        // //         >= []( auto call, auto, auto factor )
                        // //             {
                        // //                 std::cout << "Call Multiply\n";
                        // //                 return Warp::CompilerRuntime::CallType{ 
                        // //                         call.identifier, 
                        // //                         subsume_expression< FactorMultiply >( 
                        // //                                 *call.arguments.get_end(), 
                        // //                                 factor
                        // //                             )
                        // //                     };
                        // //             }, 
                        // // non_terminal_term< Call >( non_terminal_term< Call >, term< FactorDivide >, non_terminal_term< Factor > )
                        // //         >= []( auto call, auto, auto factor )
                        // //             {
                        // //                 std::cout << "Call Divide\n";
                        // //                 return Warp::CompilerRuntime::CallType{ 
                        // //                         call.identifier, 
                        // //                         subsume_expression< FactorDivide >( 
                        // //                                 *call.arguments.get_end(), 
                        // //                                 factor
                        // //                             )
                        // //                     };
                        // //             }, 
                        // non_terminal_term< CallNode >( non_terminal_term< Call >, term< CloseParenthesis > ) // non_terminal_term< Factor >, term< FactorMultiply >, non_terminal_term< Call >, term< CloseParenthesis > )
                        //         >= []( auto call, auto ) 
                        //             {
                        //                 std::cout << "Call Done.\n";
                        //                 // return Warp::Utilities::allocate_node< FactorMultiply >( 
                        //                     // factor, 
                        //                     return Warp::Utilities::allocate_node< 
                        //                                     Warp::AbstractSyntaxTree::NodeType::FunctionCall >( 
                        //                             call.identifier, 
                        //                             call.arguments
                        //                         // )
                        //                     );
                        //             }//, 
                        // // non_terminal_term< Expression >( non_terminal_term< CallNode > )
                        // //         >= []( auto call ) {
                        // //             return call;
                        // //         }

                        //////////////////////////////// Functions::Alternatives ////////////////////////////////

                        //////////////////////////////// Functions (The final boss) ////////////////////////////////                        

                        // non_terminal_term< Parameter >( term< Identifier >, non_terminal_term< LogicalOperation > )

                )
        );
    };
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP
