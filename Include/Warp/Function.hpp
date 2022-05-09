#include <Warp/AbstractSyntaxTreeUtilities.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_FUNCTION_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_FUNCTION_HPP

namespace Warp::CompilerRuntime
{
    struct Function;

    struct Parameter {
        // Warp::Utilities::HeapStringType identifier;
        Warp::Utilities::HashedStringType identifier;
        Warp::AbstractSyntaxTree::NodeVariantType constraints;
    };

    struct FunctionAlternative
    {
        const Function& function;
        const Warp::AbstractSyntaxTree::NodeVariantType expression;
        const Warp::AbstractSyntaxTree::NodeVariantType return_constraint;
        const std::vector< Parameter > input_constraints;
        const std::vector< const Function* > dependancies;
        FunctionAlternative( 
                const Function& function, 
                const Warp::AbstractSyntaxTree::NodeVariantType expression, 
                const Warp::AbstractSyntaxTree::NodeVariantType return_constraint, 
                const std::vector< Parameter > input_constraints, 
                const std::vector< const Function* > call 
            ) : 
                function( function ), 
                expression( expression ), 
                return_constraint( return_constraint ), 
                input_constraints( input_constraints ) {}
    };

    struct Function {
        // Warp::Utilities::HeapStringType identifier;
        Warp::Utilities::HashedStringType identifier;
        std::vector< FunctionAlternative > variants;
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
        std::vector< Function > functions;
        std::vector< Warp::AbstractSyntaxTree::NodeVariantType > expressions, input_constraints, output_constraints;
    };
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_FUNCTION_HPP
