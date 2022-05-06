#! /bin/python3

import sys
from pathlib import Path

DEFAULT_FILE = "simplify_log.txt"

REPLACE_LIST = {
    "'" : '', 
    "Warp::Utilities::NotSoUniquePointer<Warp::Utilities::AutoVariant<Warp::AbstractSyntaxTree::Node<Warp::Parser::ExpressionOperator::FactorMultiply>, Warp::AbstractSyntaxTree::Node<Warp::Parser::ExpressionOperator::FactorDivide>, Warp::AbstractSyntaxTree::Node<Warp::Parser::ExpressionOperator::SumAdd>, Warp::AbstractSyntaxTree::Node<Warp::Parser::ExpressionOperator::SumSubtract>, Warp::AbstractSyntaxTree::Node<Warp::AbstractSyntaxTree::NodeType::Literal>, Warp::AbstractSyntaxTree::Node<Warp::AbstractSyntaxTree::NodeType::BooleanLiteral>, Warp::AbstractSyntaxTree::Node<Warp::AbstractSyntaxTree::NodeType::Identifier>, Warp::AbstractSyntaxTree::Node<Warp::Parser::BooleanOperator::LogicalAnd>, Warp::AbstractSyntaxTree::Node<Warp::Parser::BooleanOperator::LogicalOr>, Warp::AbstractSyntaxTree::Node<Warp::Parser::BooleanOperator::LogicalBiConditional>, Warp::AbstractSyntaxTree::Node<Warp::Parser::BooleanOperator::LogicalImplies> > >" : "NodeVariant", 
    #"Warp::Utilities::NotSoUniquePointer<Warp::Utilities::AutoVariant<Warp::AbstractSyntaxTree::Node<Warp::Parser::ExpressionOperator::FactorMultiply>, Warp::AbstractSyntaxTree::Node<Warp::Parser::ExpressionOperator::FactorDivide>, Warp::AbstractSyntaxTree::Node<Warp::Parser::ExpressionOperator::SumAdd>, Warp::AbstractSyntaxTree::Node<Warp::Parser::ExpressionOperator::SumSubtract>, Warp::AbstractSyntaxTree::Node<Warp::AbstractSyntaxTree::NodeType::Literal> >" : "NodeVariantType", 
    "Warp::Parser::ExpressionOperator::" : '', 
    "Warp::AbstractSyntaxTree::" : '', 
    "Warp::AbstractSyntaxTree::NodeType::" : '' 
}

def main(): 
    log_file_path = str( Path( sys.argv[ -1 ] if len( sys.argv ) > 1 else DEFAULT_FILE ) )
    log = ""
    with open( log_file_path, 'r' ) as log_file_stream: 
        log = '\n'.join( log_file_stream.readlines() )
    #print( log )
    with open( log_file_path + ".cpp", 'w' ) as log_file_stream: 
        for key in REPLACE_LIST: 
            log = log.replace( key, REPLACE_LIST[ key ] )
        log_file_stream.write( log )

if __name__ == "__main__": 
    main()
