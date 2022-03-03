#include <iostream>
#include <ctpg.hpp>

constexpr char natural_number_regex[] = "[0-9][0-9]*";
constexpr ctpg::regex_term< natural_number_regex > natural_number_term( "NaturalNumber" );

constexpr char factor_regex[] = "[*/]";
constexpr ctpg::regex_term< factor_regex > factor_operator_term( "FactorOperator" );

constexpr ctpg::nterm< size_t > factor( "Factor" );


// This function was "yoiked" directly from https://github.com/peter-winter/ctpg //
constexpr size_t to_size_t( std::string_view integer_token )
{
    size_t sum = 0;
    for( auto digit : integer_token )
        sum = ( sum * 10 ) + digit - '0';
    return sum;
}

template< auto ValueParameterConstant >
struct ValueContainer {
    constinit static auto ValueConstant = ValueParameterConstant;
};

template< auto SearchParameterConstant, auto CurrentTermParameterConstant, auto... TapeParameterConstants >
struct IsInTemplate {
    constexpr static bool IsInCollectionConstant = IsInTemplate< SearchParameterConstant, TapeParameterConstants... >::IsInCollectionConstant;
};

template< auto SearchParameterConstant, auto... TapeParameterConstants >
struct IsInTemplate< SearchParameterConstant, SearchParameterConstant, TapeParameterConstants... > {
    constexpr static bool IsInCollectionConstant = true;
};

template< auto SearchParameterConstant, auto CurrentTermParameterConstant >
struct IsInTemplate< SearchParameterConstant, CurrentTermParameterConstant > {
    constexpr static bool IsInCollectionConstant = false;
};

template< auto... TermParameterConstants >
struct TermBuffer
{
    template< 
            template< auto... > typename TermBufferParameterType, 
            auto... ExtractedParameterConstants 
        >
    constexpr static auto Append( TermBufferParameterType< ExtractedParameterConstants... > ) {
        return TermBuffer< TermParameterConstants..., ExtractedParameterConstants... >{};
    }
    constexpr static auto ToTerms( auto... other_terms ) {
        return ctpg::terms( TermParameterConstants..., other_terms... );
    }
    template< auto TermParameterConstant >
    constexpr static auto GetTerm() {
            static_assert( IsInTemplate< TermParameterConstant, TermParameterConstants... >::IsInCollectionConstant );
            return TermParameterConstant;
    }

};

constexpr TermBuffer< '/', '*' > factor_operators;
constexpr TermBuffer< '+', '-' > sum_operators;
constexpr TermBuffer< '^' > exponent_operators;
constexpr auto expression_operators = factor_operators.Append( sum_operators ).Append( exponent_operators );


constexpr ctpg::parser factor_parser( 
        factor, 
        // ctpg::terms( '*', '/', natural_number_term ), 
        expression_operators.ToTerms( natural_number_term ), 
        ctpg::nterms( factor ), 
        ctpg::rules(
                factor( natural_number_term ) >= to_size_t, 
                factor( factor, expression_operators.GetTerm< '*' >(), natural_number_term ) 
                        >= []( size_t sum, auto factor_operator, const auto& next_token ) { // This lambda also yoikned. //
                                return sum * to_size_t( next_token ); 
                            }, 
                factor( factor, expression_operators.GetTerm< '/' >(), natural_number_term ) 
                        >= []( size_t sum, auto factor_operator, const auto& next_token ) { // This lambda also yoikned. //
                                return sum / to_size_t( next_token ); 
                            }
            )
);

int main( int argc, char** args )
{

    auto parse_result = factor_parser.parse( 
            ctpg::buffers::string_buffer( args[ 1 ] ), std::cerr 
        );
    if( parse_result.has_value() == true )
        std::cout << parse_result.value() << "\n";
    else
        std::cerr << "Error failed to parse!\n";
    return 0;
}
