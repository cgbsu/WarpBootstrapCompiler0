#include <Warp/Parsing/Terms.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_MODULES_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_MODULES_HPP

namespace Warp::Parser
{
	
	template< typename ParserParameterType, auto ProductionTypeParameterConstant >
	constexpr static const auto non_terminal_term = ParserParameterType::template non_terminal_term< ProductionTypeParameterConstant >;

	template< typename ParserParameterType, auto TokenParameterConstant >
	constexpr static const auto term = ParserParameterType::template term< TokenParameterConstant >;

	#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_MODULES_HPP_CONVIENCE( PARSER_TYPE ) \
        using enum Parser::ExpressionOperator; \
        using enum Parser::ScopeOperators; \
        using enum Parser::RegexLiteralTerms; \
        using enum Parser::StringTerms; \
        using enum Parser::NonTerminalTerms; \
        using enum Parser::BooleanOperator; \
        using enum Parser::ComparisonOperator; \
        using enum Parser::FunctionOperators; \
		using enum Parser::MetaOperators; \
		using ParserType = PARSER_TYPE;

	template< typename ParserParameterType >
	consteval auto module_declare_rule()
	{
		WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_MODULES_HPP_CONVIENCE( ParserParameterType );
		return non_terminal_term< ParserType, ModuleNameDeclaration >( term< ParserType, KeywordModule >, term< ParserType, Identifier > )
				>= []( auto, auto name_base )
				{
					return Warp::Utilities::allocate_node< 
							Warp::AbstractSyntaxTree::NodeType::ModuleDeclaration 
						>( std::vector{ std::string{ name_base } } );
				};
	}
	
	template< typename ParserParameterType >
	static consteval auto module_name_buffer_rule()
	{
		WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_MODULES_HPP_CONVIENCE( ParserParameterType );
		return non_terminal_term< ParserType, ModuleNameDeclaration >( non_terminal_term< ParserType, ModuleNameDeclaration >, term< ParserType, ScopeResolutionOperator >, term< ParserType, Identifier > )
				>= []( const auto& module_declaration, auto, auto identifier ) {
					module_declaration.namespaces.push_back( identifier );
					return module_declaration;
				};
	}

	template< typename ParserParameterType >
	static consteval auto module_declaration_rule()
	{
		WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_MODULES_HPP_CONVIENCE( ParserParameterType );
		return non_terminal_term< ParserType, ModuleDeclaration >( non_terminal_term< ParserType, ModuleDeclaration >, term< ParserType, FunctionDefintionComplete > )
				>= []( const auto& module_declaration, auto ) {
					return module_declaration;
				};
	}
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_MODULES_HPP

