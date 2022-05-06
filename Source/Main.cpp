#include <Warp/Parser.hpp>
#include <Warp/ExpressionAnalysis.hpp>

constexpr static const auto comma = ctpg::char_term( ',' );
constexpr static const auto num = Warp::Parser::Term< Warp::Parser::RegexLiteralTerms::NaturalNumber >::term;
constexpr static const char test[] = "List";
// using ListType = Warp::Utilities::VectorType< size_t >;

using ListType = Warp::Utilities::NotSoUniquePointer< std::vector< size_t > >;

constexpr static const auto list = ctpg::nterm< ListType >( test );
constexpr static const auto parser = ctpg::parser(
    list, 
    ctpg::terms( 
        comma, num 
    ), 
    ctpg::nterms( 
        list
    ), 
    ctpg::rules( 
        list( num )
        >= []( auto number ) {
            return ListType{ 
                    std::in_place_type_t< std::vector< size_t > >{}, 
                    std::initializer_list{ Warp::Utilities::to_integral< size_t >( number ) } 
                };
            // return std::move( ListType{ Warp::Utilities::to_integral< size_t >( number ) } );
        }, 
        list( list, comma, num )
        >= []( auto&& list, auto, auto number ) {
            list->push_back( Warp::Utilities::to_integral< size_t >( number ) );
            return list;
            // return std::move( ListType{ list, Warp::Utilities::to_integral< size_t >( number ) } );
        }
    )
);

int main( int argc, char** args )
{
    for( std::string input = "2"; input != "thanks :)"; std::getline( std::cin, input ) )
    {
        std::cout << "Got: " << input << "\n";
        if( auto parse_result = parser.parse( 
                            // ctpg::parse_options{}.set_verbose(), 
                            ctpg::buffers::string_buffer( input.c_str() ), std::cerr 
                    ); 
                    parse_result.has_value() == true 
                )
        {
            auto list = parse_result.value();
            std::cout << "\nResult: \n";
            const size_t list_size = list->size();
            for( size_t ii = 0; ii < list_size; ++ii )
            {
                if( ii == ( list_size - 1 ) )
                    std::cout << list->at( ii ) << "\n";
                else
                    std::cout << list->at( ii ) << ", ";
            }
        }
        else
            std::cerr << "Error failed to parse!\n";
        std::cout << "Enter prompt please: ";
    }
    return 0;
}

// int main( int argc, char** args )
// {
//     using ParserType = typename Warp::Parser::WarpParser< Warp::Parser::DefaultTypes >;
//     const auto& parser = ParserType::parser;
//     std::cout << "Hello world! Please enter your expression: ";
//     for( std::string input = "2+2"/*"4*10+2"*/; input != "thanks :)"; std::getline( std::cin, input ) )
//     {
//         std::cout << "Got: " << input << "\n";
//         if( auto parse_result = parser.parse( 
//                             // ctpg::parse_options{}.set_verbose(), 
//                             ctpg::buffers::string_buffer( input.c_str() ), std::cerr 
//                     ); 
//                     parse_result.has_value() == true 
//                 )
//         {
//             auto data = Warp::Analysis::abstract_syntax_tree_callback< Warp::Analysis::Computer, size_t >( parse_result.value() );
//             std::cout << "\nResult: " 
//                     << data 
//                     << "\n\n";
//         }
//         else
//             std::cerr << "Error failed to parse!\n";
//         std::cout << "Enter prompt please: ";
//     }
//     return 0;
// }
