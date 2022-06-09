// For the reason this file exists see: https://github.com/peter-winter/ctpg/issues/46 //
// ... Im sorry ... //

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_BOOLEAN_EXPRESSION_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_BOOLEAN_EXPRESSION_HPP

// Helper macros

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


//////////////////////////////// Boolean Expressions ////////////////////////////////

#define WARP_BOOTSTRAP_COMPILER_PARSER_RULES_BOOLEAN_EXPRESSIONS \
	/* And::Initial_Conditions */ \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( LogicalOperation, And, LogicalOperation ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( LogicalOperation, And, Comparison ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( Comparison, And, Comparison ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( Comparison, And, LogicalOperation ), \
	/* And::Chaining */ \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanAnd, And, LogicalOperation ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanAnd, And, Comparison ), \
	/* Or::Initial_Conditions */ \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( LogicalOperation, Or, LogicalOperation ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( Comparison, Or, LogicalOperation ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( Comparison, Or, Comparison ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( LogicalOperation, Or, Comparison ), \
	/* Or::Chaining::And */ \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanAnd, Or, BooleanAnd ), \
	/* Or::Chaining::LogicalOperation */ \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanOr, Or, LogicalOperation ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanAnd, Or, LogicalOperation ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( LogicalOperation, Or, BooleanAnd ), \
	/* Or::Chaning::Comparison */ \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanOr, Or, Comparison ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( BooleanAnd, Or, Comparison ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION_UNALIGNED( Comparison, Or, BooleanAnd ), \
	/*Extended Logical (evalated last before aritmatic) */ \
	/* TODO: ADD BOOLEAN LOGICAL TESTS!!!!!! */ \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION( LogicalOperation, BiCondition, LogicalOperation ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION( LogicalOperation, Implies, LogicalOperation ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION( LogicalOperation, BiCondition, Comparison ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION( LogicalOperation, Implies, Comparison ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION( Comparison, BiCondition, Comparison ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION( Comparison, Implies, Comparison ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION( Comparison, BiCondition, LogicalOperation ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_LOGICAL_OPERATION( Comparison, Implies, LogicalOperation ), \
	/* Not */ \
	non_terminal_term< LogicalOperation >( term< LogicalNot >, non_terminal_term< LogicalOperation > )\
	        >= []( auto, auto logical_expression ) {\
	            return Warp::Utilities::allocate_node< LogicalNot >( logical_expression );\
	        }, \
	/* This seems to do the trick with reducing the not's just down to one or two depending on if there are an even or odd amount. */\
	non_terminal_term< LogicalOperation >( term< LogicalNot >, term< LogicalNot >, non_terminal_term< LogicalOperation > )\
	        >= []( auto, auto, auto logical_expression ) {\
	            return logical_expression;\
	        }, \
	/* No other logical terms here, just return what you've got. */\
	non_terminal_term< LogicalOperation >( non_terminal_term< BooleanOr > )\
	        >= []( auto or_expression ) {\
	            return or_expression;\
	        }, \
	non_terminal_term< LogicalOperation >( non_terminal_term< BooleanAnd > )\
	        >= []( auto and_expression ) {\
	            return and_expression;\
	        },                         \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_REDUCE_MIDDLE_OF_PARENTHESIS_TO( LogicalOperation, LogicalOperation ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_REDUCE_MIDDLE_OF_PARENTHESIS_TO( LogicalOperation, BooleanAnd ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_REDUCE_MIDDLE_OF_PARENTHESIS_TO( LogicalOperation, BooleanOr )
	
//////////////////////////////// Comparison Expressions ////////////////////////////////
#define WARP_BOOTSTRAP_COMPILER_PARSER_RULES_COMPARISON_EXPRESSIONS \
	/* Comparisions must return a Comparison non terminal term so they can be evaluated before the logical expressions (and thus used in them) */\
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON( ComparisonEqual ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON( ComparisonLessThan ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON( ComparisonGreaterThan ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON_UNALIGNED( GreaterThanOrEqualTo, ComparisionGreaterThanOrEqualTo ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_COMPARISON_UNALIGNED( LessThanOrEqualTo, ComparisionLessThanOrEqualTo ), \
	WARP_BOOTSTRAP_COMPILER_HEADER_PARSER_HPP_REDUCE_MIDDLE_OF_PARENTHESIS_TO( LogicalOperation, Comparison ) \
	/* TODO: Add ternary/chaining comparisons and : "piping/forwarding" */
	
#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_BOOLEAN_EXPRESSION_HPP
	
