#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_FUNCTIONS_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_FUNCTIONS_HPP

//////////////////////////////// Functions::Parameters ////////////////////////////////


#define WARP_BOOTSTRAP_COMPILER_PARSER_RULES_FUNCTION_PARAMETERS \
	/* TODO: Add types/concepts with additional colon, e.g my_parameter : natural : my_parameter < 128 */ \
	/* TODO: Update now considering this exceot with the syntax identifier< concepts/types | concepts/types > : constraints */ \
	non_terminal_term< ParameterList >( non_terminal_term< ParameterList >, term< Identifier >, term< FunctionParameterConstaraint >, non_terminal_term< Comparison > ) \
	        >= []( auto parameter_list, auto parameter_name, auto, auto comparison_constraint ) { \
	                return add_to_parameter_list( parameter_list, parameter_name, comparison_constraint ); \
	        },  \
	non_terminal_term< ParameterList >( non_terminal_term< ParameterList >, term< Identifier >, term< FunctionParameterConstaraint >, non_terminal_term< LogicalOperation > ) \
	        >= []( auto parameter_list, auto parameter_name, auto, auto constraints ) { \
	                return add_to_parameter_list( parameter_list, parameter_name, constraints ); \
	        }, \
	non_terminal_term< ParameterList >( non_terminal_term< ParameterList >, term< Identifier >, term< FunctionParameterConstaraint >, non_terminal_term< BooleanAnd > ) \
	        >= []( auto parameter_list, auto parameter_name, auto, auto constraints ) { \
	                return add_to_parameter_list( parameter_list, parameter_name, constraints ); \
	        }, \
	non_terminal_term< ParameterList >( non_terminal_term< ParameterList >, term< Identifier >, term< FunctionParameterConstaraint >, non_terminal_term< BooleanOr > ) \
	        >= []( auto parameter_list, auto parameter_name, auto, auto constraints ) { \
	                return add_to_parameter_list( parameter_list, parameter_name, constraints ); \
	        },  \
	non_terminal_term< ParameterList >( non_terminal_term< ParameterList >, term< Identifier >, term< FunctionParameterNextParameter > ) \
	        >= []( auto parameter_list, auto parameter_name, auto ) \
			{ \
	                return add_to_parameter_list(  \
								parameter_list,  \
								parameter_name,  \
								Warp::Utilities::allocate_node< Warp::AbstractSyntaxTree::NodeType::Unconstrained >()  \
							); \
	        },  \
	non_terminal_term< ParameterList >( non_terminal_term< ParameterList >, term< Identifier > ) \
	        >= []( auto parameter_list, auto parameter_name ) \
			{ \
	            return add_to_parameter_list(  \
							parameter_list,  \
							parameter_name,  \
							Warp::Utilities::allocate_node< Warp::AbstractSyntaxTree::NodeType::Unconstrained >()  \
						); \
	        },  \
	non_terminal_term< ParameterList >( non_terminal_term< ParameterList >, term< FunctionParameterNextParameter > ) \
	        >= []( auto parameter_list, auto ) { \
	            return parameter_list; \
	        },  \
	non_terminal_term< ParameterList >( non_terminal_term< ParameterList >, term< FunctionParameterNextParameter > ) \
	        >= []( auto parameter_list, auto ) { \
	            return parameter_list; \
	        },  \
	non_terminal_term< ParameterList >( term< KeywordLet >, term< Identifier >, term< OpenParenthesis > ) \
	        >= []( auto let, auto function_name, auto open_parenthesis ) \
	        { \
	            return Warp::CompilerRuntime::IntermediateFunctionAlternative{  \
	                    Warp::Utilities::hash_string( function_name ),  \
	                    std::string{ function_name } \
	                }; \
	        }, \
	non_terminal_term< Arguments >( non_terminal_term< ParameterList >, term< CloseParenthesis > ) \
	        >= []( auto parameter_list, auto ) { \
	            return parameter_list; \
	        }

#define WARP_BOOTSTRAP_COMPILER_PARSER_RULES_FUNCTION_DEFEINITIONS \
	non_terminal_term< ExpressionEater >( non_terminal_term< Arguments >, term< ScopeResolutionOperator >, non_terminal_term< Expression > ) \
	        >= []( auto arguments, auto, auto expression ) \
	        { \
	            return Warp::CompilerRuntime::FunctionAlternative{  \
	                arguments.identifier,  \
	                expression,  \
	                Warp::Utilities::allocate_node< Warp::AbstractSyntaxTree::NodeType::Unconstrained >(), /* No return constraint found. */ \
	                arguments.input_constraints,  \
	                arguments.name  \
	                /* dependancies would go here */ \
	            }; \
	        },  \
	non_terminal_term< ExpressionEater >( non_terminal_term< Arguments >, non_terminal_term< Comparison >, term< ScopeResolutionOperator >, non_terminal_term< Expression > ) \
	        >= []( auto arguments, auto return_constraint, auto, auto expression ) \
	        { \
	            return Warp::CompilerRuntime::FunctionAlternative{  \
	                arguments.identifier,  \
	                expression,  \
	                return_constraint,  \
	                arguments.input_constraints,  \
	                arguments.name  \
	                /* dependancies would go here */ \
	            }; \
	        },  \
	non_terminal_term< ExpressionEater >( non_terminal_term< Arguments >, non_terminal_term< LogicalOperation >, term< ScopeResolutionOperator >, non_terminal_term< Expression > ) \
	        >= []( auto arguments, auto return_constraint, auto, auto expression ) \
	        { \
	            return Warp::CompilerRuntime::FunctionAlternative{  \
	                arguments.identifier,  \
	                expression,  \
	                return_constraint,  \
	                arguments.input_constraints,  \
	                arguments.name  \
	                /* dependancies would go here */ \
	            }; \
	        },  \
	non_terminal_term< WarpFunctionAlternative >( non_terminal_term< ExpressionEater >, term< FunctionDefintionComplete > )  \
	        >= []( auto function, auto ) { \
	                return function; \
	            } 


    //////////////////////////////// Functions::Calls A Hard Won Battle ////////////////////////////////

#define WARP_BOOTSTRAP_COMPILER_PARSER_RULES_FUNCTION_CALLS \
    non_terminal_term< Call >( term< Identifier >, term< OpenParenthesis > )  \
            >= []( auto identifier, auto )  \
                { \
                    return Warp::CompilerRuntime::CallType{  \
                            Warp::Utilities::hash_string( identifier ),  \
                            std::string{ identifier },  \
                            std::vector< Warp::AbstractSyntaxTree::NodeVariantType >{} \
                        }; \
                },  \
    non_terminal_term< Call >( non_terminal_term< Call >, non_terminal_term< Factor >, term< FunctionParameterNextParameter > )  \
            >= []( auto call, auto argument, auto ) { \
                    call.arguments.push_back( argument ); \
                    return call; \
                },  \
    non_terminal_term< CallNode >( non_terminal_term< Call >, non_terminal_term< Factor >, term< CloseParenthesis > )  \
            >= []( auto call, auto argument, auto ) \
                { \
                    call.arguments.push_back( argument ); \
                    return Warp::Utilities::allocate_node< Warp::AbstractSyntaxTree::NodeType::FunctionCall >(  \
                            call.identifier,  \
                            call.function_name,  \
                            call.arguments  \
                        ); \
                },  \
    non_terminal_term< Call >( non_terminal_term< Call >, non_terminal_term< CallNode >, term< FunctionParameterNextParameter > )  \
            >= []( auto call, auto argument, auto ) { \
                    call.arguments.push_back( argument ); \
                    return call; \
                },  \
    non_terminal_term< CallNode >( non_terminal_term< Call >, non_terminal_term< CallNode >, term< CloseParenthesis > )  \
            >= []( auto call, auto argument, auto ) \
                { \
                    return Warp::Utilities::allocate_node< Warp::AbstractSyntaxTree::NodeType::FunctionCall >(  \
                            call.identifier,  \
                            call.function_name,  \
                            call.arguments  \
                        ); \
                },  \
    non_terminal_term< CallNode >( non_terminal_term< Call >, term< CloseParenthesis > )  \
            >= []( auto call, auto ) \
                { \
                    return Warp::Utilities::allocate_node< Warp::AbstractSyntaxTree::NodeType::FunctionCall >(  \
                            call.identifier,  \
                            call.function_name,  \
                            call.arguments  \
                        ); \
                },  \
    non_terminal_term< Factor >( non_terminal_term< CallNode > ) \
            >= []( auto call ) { \
                    return call; \
                },  \
    non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorMultiply >, non_terminal_term< CallNode > ) \
            >= []( auto factor, auto, auto call ) { \
                    return Warp::Utilities::allocate_node< FactorMultiply >(  \
                            factor,  \
                            call  \
                        ); \
                },  \
    non_terminal_term< Factor >( non_terminal_term< Factor >, term< FactorDivide >, non_terminal_term< CallNode > ) \
            >= []( auto factor, auto, auto call ) { \
                    return Warp::Utilities::allocate_node< FactorMultiply >(  \
                            factor,  \
                            call  \
                        ); \
                } 

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_PARSING_FUNCTIONS_HPP

