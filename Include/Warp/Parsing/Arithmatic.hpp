// For the reason this file exists see: https://github.com/peter-winter/ctpg/issues/46 //
// ... Im sorry ... //

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_ARITHMATIC_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_ARITHMATIC_HPP



//////////////////////////////// Arithmatic Expressions::Consume Values ////////////////////////////////

#define WARP_BOOTSTRAP_COMPILER_PARSER_RULES_CONSUME_VALUES \
	non_terminal_term< Factor >( term< FunctionResult > ) \
	        >= []( auto token ) { \
	            return std::move( Warp::Utilities::allocate_node< FunctionResult >() ); \
	        }, \
	non_terminal_term< Factor >( term< Identifier > ) \
	        >= []( auto token )  \
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
	        } 

//////////////////////////////// Arithmatic Expressions::Products ////////////////////////////////

#define WARP_BOOTSTRAP_COMPILER_PARSER_RULES_PRODUCTS \
	non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorMultiply >, term< NaturalNumber > )  \
	        >= []( auto current_factor, auto, const auto& next_token ) \
	        { \
	            return Warp::Utilities::allocate_node< FactorMultiply >(  \
	                    current_factor,  \
	                    Warp::Utilities::allocate_integral_literal_node<  \
	                            ResolvedType< NaturalNumber >  \
	                    >( next_token ) \
	                ); \
	        },  \
	non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorDivide >, term< NaturalNumber > )  \
	        >= []( auto current_factor, auto, const auto& next_token ) \
	        { \
	            return Warp::Utilities::allocate_node< FactorDivide >(  \
	                    current_factor,  \
	                    Warp::Utilities::allocate_integral_literal_node<  \
	                            ResolvedType< NaturalNumber >  \
	                    >( next_token ) \
	                ); \
	        },  \
	non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorMultiply >, term< Identifier > )  \
	        >= []( auto current_factor, auto, const auto& next_token ) \
	        { \
	            return Warp::Utilities::allocate_node< FactorMultiply >(  \
	                    current_factor,  \
	                    Warp::Utilities::allocate_node<  \
	                            Warp::AbstractSyntaxTree::NodeType::Identifier  \
	                    >( next_token ) \
	                ); \
	        },  \
	non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorDivide >, term< Identifier > )  \
	        >= []( auto current_factor, auto, const auto& next_token ) \
	        { \
	            return Warp::Utilities::allocate_node< FactorDivide >(  \
	                    current_factor,  \
	                    Warp::Utilities::allocate_node<  \
	                            Warp::AbstractSyntaxTree::NodeType::Identifier  \
	                    >( next_token ) \
	                ); \
	        },  \
	non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorMultiply >, term< FunctionResult > )  \
	        >= []( auto current_factor, auto, const auto& next_token ) \
	        { \
	            return Warp::Utilities::allocate_node< FactorMultiply >(  \
	                    current_factor,  \
	                    Warp::Utilities::allocate_node< FunctionResult >() \
	                ); \
	        },  \
	non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorDivide >, term< FunctionResult > )  \
	        >= []( auto current_factor, auto, const auto& next_token ) \
	        { \
	            return Warp::Utilities::allocate_node< FactorDivide >(  \
	                    current_factor,  \
	                    Warp::Utilities::allocate_node< FunctionResult >() \
	                ); \
	        } 

//////////////////////////////// Arithmatic Expressions::Parenthesis ////////////////////////////////

#define WARP_BOOTSTRAP_COMPILER_PARSER_RULES_PARENTHESIS \
	non_terminal_term< ParenthesisScope >( term< OpenParenthesis >, non_terminal_term< Factor >, term< CloseParenthesis > ) \
	        >= []( auto, auto factor, auto ) { return factor; },  \
	non_terminal_term< Factor >( non_terminal_term< ParenthesisScope > )  \
	        >= []( auto parenthesis_scope ) { return parenthesis_scope; },  \
	non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorMultiply >, non_terminal_term< ParenthesisScope > )  \
	        >= []( auto factor, auto, auto parenthesis_scope )  \
	            { \
	                return Warp::Utilities::allocate_node< FactorMultiply >(  \
	                        factor,  \
	                        parenthesis_scope  \
	                    ); \
	            },  \
	non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorDivide >, non_terminal_term< ParenthesisScope > )  \
	        >= []( auto factor, auto, auto parenthesis_scope )  \
	            { \
	                return Warp::Utilities::allocate_node< FactorDivide >(  \
	                        factor,  \
	                        parenthesis_scope  \
	                    ); \
	            } 
 
//////////////////////////////// Arithmatic Expressions::Sums ////////////////////////////////

#define WARP_BOOTSTRAP_COMPILER_PARSER_RULES_SUMS \
	non_terminal_term< Factor >( non_terminal_term< Sum > )  \
	        >= []( auto sum ) { \
	            return sum; \
	        },  \
	non_terminal_term< Sum >( non_terminal_term< Factor >, term< SumAdd >, non_terminal_term< Factor > )  \
	        >= []( auto current_sum, auto, const auto& next_token )  \
	            { \
	                return Warp::Utilities::allocate_node< SumAdd >(  \
	                        current_sum,  \
	                        next_token  \
	                    ); \
	            },  \
	non_terminal_term< Sum >( non_terminal_term< Factor >, term< SumSubtract >, non_terminal_term< Factor > )  \
	        >= []( auto current_sum, auto, const auto& next_token ) \
	            { \
	                return Warp::Utilities::allocate_node< SumSubtract >(  \
	                        current_sum,  \
	                        next_token  \
	                    ); \
	            },  \
	non_terminal_term< Expression >( non_terminal_term< Factor > )  \
	        >= []( auto factor ) { \
	            return factor; \
	        } 

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_ARITHMATIC_HPP

