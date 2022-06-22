#include <Warp/Parsing/Terms.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_LITERALS_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_LITERALS_HPP

namespace Warp::Parser
{
	template< 
			auto TypeParameterConstant, 
			typename ParserParameterType 
		>
	struct LiteralGenerator {
		LiteralGenerator() = delete;
	};
	
	template< typename ParserParameterType >
	struct LiteralGenerator< 
			FunctionOperators::FunctionResult, 
			ParserParameterType 
		> 
	{
		static constexpr auto generate_literal( auto token ) {
			return std::move( Warp::Utilities::allocate_node< 
					FunctionOperators::FunctionResult
					//NonTerminalTerms::WarpFunctionAlternative 
				>() );
		}
	};
	
	template< typename ParserParameterType >
	struct LiteralGenerator< 
			RegexLiteralTerms::Identifier, 
			ParserParameterType 
		> 
	{
		static constexpr auto generate_literal( auto token )
		{
			return std::move( Warp::Utilities::allocate_node< 
			        Warp::AbstractSyntaxTree::NodeType::Identifier 
			    >( token ) );
		}
	};
	
	template< typename ParserParameterType >
	struct LiteralGenerator< 
			RegexLiteralTerms::NaturalNumber, 
			ParserParameterType 
		> 
	{
		static constexpr auto generate_literal( auto token )
		{
		    return std::move( Warp::Utilities::allocate_integral_literal_node< 
					typename ParserParameterType::ResolvedType< RegexLiteralTerms::NaturalNumber >
				>( token ) );
		}
	};

	template< typename ParserParameterType >
	struct LiteralGenerator< 
			RegexLiteralTerms::BooleanLiteral, 
			ParserParameterType 
		> 
	{
		static constexpr auto generate_literal( auto token ) {
		    return Warp::Utilities::allocate_boolean_literal_node( token );
		}
	};

	template< 
			typename ParserParameterType, 
			auto ProductionTypeParameterConstant, 
			auto TokenTypeParameterConstants 
		>
	constexpr auto make_literal_rule()
	{ 
		using ParserType = ParserParameterType;
		return ParserType::template non_terminal_term< ProductionTypeParameterConstant >( 
						ParserType::template term< TokenTypeParameterConstants > ) 
				>= []( auto tokens ) 
				{
					return LiteralGenerator< 
							TokenTypeParameterConstants, 
							ParserType
						>{}.generate_literal( tokens );
				};
	};
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_LITERALS_HPP

