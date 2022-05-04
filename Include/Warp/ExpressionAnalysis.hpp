#include <Warp/AbstractSyntaxTreeUtilities.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_ANYLSIS_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_EXPRESSION_ANYLSIS_HPP

namespace Warp::Analysis
{
    template< auto FeedbackParameterConstant >
    auto abstract_syntax_tree_callback( const Warp::AbstractSyntaxTree::VariantType& variant )
    {
            const typename std::remove_pointer< decltype( variant.get_pointer() ) >::type& node = *variant.get_pointer();
            return Warp::Utilities::visit< []( auto x ) { 
                    return FeedbackParameterConstant( *x ); 
                }>( node );
    }

}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_HPP
