#include <Warp/AbstractSyntaxTreeUtilities.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_FUNCTION_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_FUNCTION_HPP

namespace Warp::CompilerRuntime
{
    using CallType = Warp::AbstractSyntaxTree::Node< Warp::AbstractSyntaxTree::NodeType::FunctionCall >
    ;
    struct Function;

    struct Parameter {
        Warp::Utilities::HashedStringType identifier;
        Warp::AbstractSyntaxTree::NodeVariantType constraints;
    };

    struct IntermediateFunctionAlternative 
    {
        Warp::Utilities::HashedStringType identifier;
        std::string name;        
        std::vector< Parameter > input_constraints;
    };

    struct FunctionAlternative
    {
        // Function* function; // TEMPORARY
        Warp::Utilities::HashedStringType identifier; // TEMPORARY
        Warp::AbstractSyntaxTree::NodeVariantType expression;
        Warp::AbstractSyntaxTree::NodeVariantType return_constraint;
        std::vector< Parameter > input_constraints;
        std::string name;
        // std::vector< Function* > dependancies;
    };

    struct FunctionAlternatives {
        std::vector< FunctionAlternative* > alternatives;
        size_t number_of_parameters;
    };

    struct Function
    {
        Warp::Utilities::HashedStringType identifier;
        std::string name;
        std::vector< FunctionAlternatives > alternatives;
    };

    struct LogEntry
    {
        const size_t function;
        const size_t variant;
        const std::vector< Warp::AbstractSyntaxTree::LiteralType > inputs;
        const Warp::AbstractSyntaxTree::LiteralType output;
    };
    
    struct Log {
        std::vector< LogEntry > forward_inputs, backward_propigation;
    };

    struct Module {
        std::vector< Function* > functions;
        // std::vector< Warp::AbstractSyntaxTree::NodeVariantType > expressions, input_constraints, output_constraints;
    };
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_FUNCTION_HPP
