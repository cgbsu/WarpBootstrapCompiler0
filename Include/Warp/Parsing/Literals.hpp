#include <Warp/Parsing/Terms.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_LITERALS_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_LITERALS_HPP

namespace Warp::Parser
{
	// For the reason this file exists see: https://github.com/peter-winter/ctpg/issues/46 //
	
	
	template< auto TypeParameterConstant >
	struct LiteralGenerator {
		LiteralGenerator() = delete;
	};
	
	template<>
	struct LiteralGenerator< FunctionResult >
	{
		static constexpr auto generate_literal( auto token ) {
			return std::move( Warp::Utilities::allocate_node< FunctionResult >() );
		}
	};
	
	template<>
	struct LiteralGenerator< Warp::AbstractSyntaxTree::NodeType::Identifier >
	{
		static constexpr auto generate_literal( auto token )
		{
			return std::move( Warp::Utilities::allocate_node< 
			                Warp::AbstractSyntaxTree::NodeType::Identifier 
			        >( token ) );
		}
	};
	
	template<>
	struct LiteralGenerator< Warp::Parser::RegexLiteralTerms::NaturalNumber > 
	{
		static constexpr auto generate_literal( auto token )
		{
		    return std::move( Warp::Utilities::allocate_integral_literal_node< 
					ResolvedType< NaturalNumber > 
				>( token ) );
		}
	};

	template< 
			typename TermsParameterType, 
			typename NonTerminalParameterTypes, 
			auto ProductionTypeParameterConstant, 
			auto... TokenTypeParameterConstants 
		>
	consteval auto make_literal_rule()
	{ 
		return TermsParameterType< ProductionTypeParameterConstant >( NonTerminalParameterTypes< TokenTypeParameterConstants >... ) 
				>= []( auto... tokens ) {
					return LiteralGenerator< ProductionTypeParameterConstant >( tokens... );
				};
	}
		/*non_terminal_term< Factor >( term< FunctionResult > ) \
		        >= []( auto token ) { \
		            return std::move( Warp::Utilities::allocate_node< FunctionResult >() ); \
		        }, \
		non_terminal_term< Factor >( term< Identifier > ) \
		        >= []( auto token ) \
		        { \
		            return std::move( Warp::Utilities::allocate_node< \
		                            Warp::AbstractSyntaxTree::NodeType::Identifier \
		                    >( token ) ); \
		        }, \
		non_terminal_term< Factor >( term< NaturalNumber > ) \
		        >= []( auto token ) \
		        { \
		            return std::move( Warp::Utilities::allocate_integral_literal_node< \
		                    ResolvedType< NaturalNumber > \
		                >( token ) ); \
		        }, \
		non_terminal_term< LogicalOperation >( term< BooleanLiteral > ) \
		        >= []( auto token ) { \
		            return Warp::Utilities::allocate_boolean_literal_node( token ); \
		        } 
		*/
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_LITERALS_HPP

