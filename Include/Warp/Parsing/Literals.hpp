// For the reason this file exists see: https://github.com/peter-winter/ctpg/issues/46 //

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_LITERALS_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_LITERALS_HPP

#define WARP_BOOTSTRAP_COMPILER_PARSER_RULES_LITERALS \
	non_terminal_term< Factor >( term< FunctionResult > ) \
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

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_LITERALS_HPP

