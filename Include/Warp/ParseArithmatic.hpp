/*namespace Warp::Parser
{
		//////////////////////////////// Arithmatic Expressions::Literals ////////////////////////////////
		
		non_terminal_term< Factor >( term< FunctionResult > ) 
		        >= []( auto token ) {
		            return std::move( Warp::Utilities::allocate_node< FunctionResult >() );
		        }, 
		non_terminal_term< Factor >( term< Identifier > ) 
		        >= []( auto token ) 
		        {
		            return std::move( Warp::Utilities::allocate_node< 
		                            Warp::AbstractSyntaxTree::NodeType::Identifier 
		                    >( token ) );
		        }, 
		non_terminal_term< Factor >( term< NaturalNumber > ) 
		        >= []( auto token )
		        {
		            return std::move( Warp::Utilities::allocate_integral_literal_node< 
		                    ResolvedType< NaturalNumber > 
		                >( token ) );
		        }, 
}
*/
