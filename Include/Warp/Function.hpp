#include <Warp/AbstractSyntaxTreeUtilities.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_FUNCTION_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_FUNCTION_HPP

namespace Warp::CompilerRuntime
{
    using CallType = Warp::AbstractSyntaxTree::Node< 
			Warp::AbstractSyntaxTree::NodeType::FunctionCall 
		>;

    struct Function;

    struct Parameter
    {
        Warp::Utilities::HashedStringType identifier;
        std::string name;
        Warp::AbstractSyntaxTree::NodeVariantType constraints;
    };

    struct IntermediateFunctionAlternative 
    {
        Warp::Utilities::HashedStringType identifier;
        std::string name;        
        std::vector< Parameter > input_constraints;
    };

    struct FunctionAlternatives;
	struct Module;

    struct FunctionAlternative
    {
        // Function* function; // TEMPORARY
        Warp::Utilities::HashedStringType identifier; // TEMPORARY
        Warp::AbstractSyntaxTree::NodeVariantType expression;
        Warp::AbstractSyntaxTree::NodeVariantType return_constraint;
        std::vector< Parameter > input_constraints;
        std::string name;
		const FunctionAlternatives& alternatives;
        // std::vector< Function* > dependancies;
    };

    struct FunctionAlternatives
	{
		const Function& function;
        const size_t number_of_parameters;
        std::vector< FunctionAlternative* > alternatives;
		FunctionAlternatives() = delete;
		FunctionAlternatives( 
				const Function& function, 
				size_t number_of_parameters 
			) : function( function ), 
				number_of_parameters( number_of_parameters ) {}
    };

    struct Function
    {
        Warp::Utilities::HashedStringType identifier;
        std::string name;
		const Module& module;
        std::vector< FunctionAlternatives > alternatives;
    };

    struct LogEntry
    {
        const size_t function;
        const size_t variant;
        const size_t number_of_parameters;
        const std::vector< AbstractSyntaxTree::ValueType > inputs;
        const AbstractSyntaxTree::ValueType output;
    };

    struct Module
    {
		std::string name;
		std::filesystem::path full_path;
        std::vector< std::unique_ptr< Function > > functions;
        // 2D vector by number of parameters, then actual entries. //
        std::vector< std::vector< LogEntry > > forward_input_log; // TODO: backward_propigation_log //
		Module( std::filesystem::path full_path ); 
		std::optional< size_t > find_function_index_in_module( std::string function_name );
		std::optional< LogEntry > log_call( 
    	        const FunctionAlternative& caller, 
    	        std::vector< AbstractSyntaxTree::ValueType >& inputs, 
    	        const AbstractSyntaxTree::ValueType result 
    	    );
	};

	/*struct ModuleTree
	{
		std::optional< std::unique_ptr< Module > > add_sub_module( std::unique_ptr< Module >  module );
		protected: 
			std::unordered_map< std::string, ModuleTree > branches;
			std::vector< std::unique_ptr< Module > > stems;
	};*/
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_FUNCTION_HPP

