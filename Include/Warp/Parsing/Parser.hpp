#include <Warp/Parsing/Terms.hpp>
#include <ctpg/ctpg.hpp>
#include <Warp/Parsing/Literals.hpp>
#include <Warp/Parsing/Arithmatic.hpp>
#include <Warp/Parsing/BooleanExpression.hpp>
#include <Warp/Parsing/Functions.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP

namespace Warp::Parser
{
    #define WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_REDUCE_MIDDLE_OF_PARENTHESIS_TO( TO, MIDDLE ) \
        non_terminal_term< TO >( term< OpenParenthesis >, non_terminal_term< MIDDLE >, term< CloseParenthesis > ) \
            >= []( auto, auto middle, auto ) { \
                return middle; \
            } 


    template< 
            template< auto > typename TypeResolverParameterType = DefaultTypes, 
            auto ReduceTo = Warp::Parser::NonTerminalTerms::WarpFunctionAlternative, 
			typename TermsParameterType = Parser::DefaultTerms::TermsType, 
			typename NonTerminalTermsParameterType = DefaultTerms::NonTerminalTermsType 
        >
    struct WarpParser
    {
		using TermsType = TermsParameterType;
		using NonTerminalTermsType = NonTerminalTermsParameterType;
		
        using enum Parser::ExpressionOperator;
        using enum Parser::ScopeOperators;
        using enum Parser::RegexLiteralTerms;
        using enum Parser::StringTerms;
        using enum Parser::NonTerminalTerms;
        using enum Parser::BooleanOperator;
        using enum Parser::ComparisonOperator;
        using enum Parser::FunctionOperators;
		using enum Parser::MetaOperators;

        template< auto ParameterConstant >
        constexpr const static auto term = TermsType::template get_term< ParameterConstant >();

        template< auto ParameterConstant >
        constexpr const static auto non_terminal_term = NonTerminalTermsType::template get_term< ParameterConstant >();

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
                        std::string{ parameter_name }, 
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
		

		// Could the problem be static linkage ? Perhapse if I moved some of this outside the class it would work? //
		consteval const static auto make_my_rules() {
			return ctpg::rules( 
					non_terminal_term< Factor >( term< '$' > ) 
						>= []( auto ) {
							return std::move( Utilities::allocate_integral_literal_node< ResolvedType< NaturalNumber > >( std::string{ "42" } ) );
						}, 
					non_terminal_term< Factor >( term< '$' >, term< '$' > ) 
						>= []( auto, auto ) {
							return std::move( Utilities::allocate_integral_literal_node< ResolvedType< NaturalNumber > >( std::string{ "64" } ) );
						} 
				);
		}

        // template< auto NodeTagParameterConstant >
        // constexpr static auto factorsubsume_function_alternative_expression( auto function, auto factor )
        
        constexpr static const auto parser = ctpg::parser( 
                non_terminal_term< ReduceTo >, 
                terms, 
                non_terminal_terms, 
				make_my_rules() 
			);
                /*std::tuple_cat( ctpg::rules( 

                        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        // CTPG Makes it very hard to seperate all these out (see: https://github.com/peter-winter/ctpg/issues/46 ) //
                        // I spent a good amount of time doings so I am going to annotate the different sections. I also have not ////
                        // yet figured out how to generate rules externally and automatically, so please excuese the macros and //////
                        // crummy code overall here, I will just have to document it a bit. //////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////////////////////////////////////////

						WARP_BOOTSTRAP_COMPILER_PARSER_RULES_LITERALS,  
						WARP_BOOTSTRAP_COMPILER_PARSER_RULES_CONSUME_VALUES,  
						WARP_BOOTSTRAP_COMPILER_PARSER_RULES_PRODUCTS,  
						WARP_BOOTSTRAP_COMPILER_PARSER_RULES_PARENTHESIS,  
						WARP_BOOTSTRAP_COMPILER_PARSER_RULES_SUMS,  
						WARP_BOOTSTRAP_COMPILER_PARSER_RULES_BOOLEAN_EXPRESSIONS, 
						WARP_BOOTSTRAP_COMPILER_PARSER_RULES_COMPARISON_EXPRESSIONS, 
                        WARP_BOOTSTRAP_COMPILER_PARSER_RULES_FUNCTION_PARAMETERS, 
						WARP_BOOTSTRAP_COMPILER_PARSER_RULES_FUNCTION_DEFEINITIONS,  
						WARP_BOOTSTRAP_COMPILER_PARSER_RULES_FUNCTION_CALLS,  
					), 
					make_my_rules() 
				) );*/

                        //////////////////////////////// Meta ////////////////////////////////

					//	non_terminal_term< MetaOperator >( term< Identifier > ) 
					//			>= []( auto identifier ) {
					//					return meta;
					//			}
    };
    using DefaultParserType = typename Warp::Parser::WarpParser< 
            Warp::Parser::DefaultTypes, 
            Warp::Parser::NonTerminalTerms::WarpFunctionAlternative 
        >;
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP

