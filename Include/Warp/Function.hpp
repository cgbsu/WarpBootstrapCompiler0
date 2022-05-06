/*namespace Warp::Functional
{
    struct Function;

    struct FunctionAlternative
    {
        const Function& function;
        const NodeNodeVariantType expression;
        const NodeNodeVariantType return_constraint;
        const Array< NodeNodeVariantType > input_constraints;
        const Array< const Function& > call;
        FunctionAlternative( 
                const Function& function, 
                const NodeNodeVariantType expression, 
                const NodeNodeVariantType return_constraint, 
                const Array< NodeNodeVariantType > input_constraints, 
                const Array< const Function& > call 
            ) : 
                function( function ), 
                expression( expression ), 
                return_constraint( return_constraint ), 
                input_constraints( input_constraints ) {}
    };

    struct Function
    {
        Warp::Utilities::HeapStringType identifier;
        Array< FunctionAlternative > variants;
    };

    struct LogEntry
    {
        const size_t function;
        const size_t variant;
        const Array< StrongFactor > inputs
        const StrongFactor output;
    };

    struct Log {
        std::vector< LogEntry > forward_inputs, backward_propigation;
    };
}*/