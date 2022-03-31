#include <iostream>
#include <functional>
#include <array>
#include <string>
#include <sstream>

#include <math.h>
#include <string.h>

#include <ctpg.hpp>
/*
template< auto FirstConstantParameter, auto... SeriesConstantParameters >
struct TakeOneFromTemplateSeries {
    constexpr static auto first = FirstConstantParameter;
    using NextType = TakeOneFromTemplateSeries< SeriesConstantParameters... >;
};

template< auto... ElementParameterConstants >
struct RawTemplateArray
{
    using ElementType = decltype( 
            TakeOneFromTemplateSeries< ElementParameterConstants... >::first 
        );
    constexpr static auto size = sizeof...( ElementParameterConstants );
    constexpr static ElementType array[ size ] = { ElementParameterConstants... };
    constexpr static ElementType* pointer = array;
};

template< 
        auto ArrayParameterConstant, 
        size_t IndexParameterConstant, 
        size_t ArrayLengthParameterConstant, 
        auto... ElementParameterConstants 
    >
struct ToRawTemplateArrayImplementation
{
    using ResultType = typename ToRawTemplateArrayImplementation< 
            ArrayParameterConstant, 
            IndexParameterConstant + 1, 
            ArrayLengthParameterConstant, 
            ElementParameterConstants..., 
            ArrayParameterConstant[ IndexParameterConstant % ArrayLengthParameterConstant ] 
        >::ResultType;
};

template< 
        auto ArrayParameterConstant, 
        size_t IndexParameterConstant, 
        auto... ElementParameterConstants 
    >
struct ToRawTemplateArrayImplementation< 
        ArrayParameterConstant, 
        IndexParameterConstant, 
        IndexParameterConstant, 
        ElementParameterConstants... 
    >
{
    using ResultType = RawTemplateArray< 
            ElementParameterConstants... 
        >;
};

template< auto ArrayParameterConstant >
struct ToRawTemplateArray
{
    using ResultType = typename ToRawTemplateArrayImplementation< 
            ArrayParameterConstant, 
            0, 
            // std::strlen( ArrayParameterConstant ) 
            ctpg::utils::str_len( ArrayParameterConstant ) + 1
        >::ResultType;
};*/


// This function was "yoiked" directly from https://github.com/peter-winter/ctpg //
constexpr size_t to_size_t( std::string_view integer_token )
{
    size_t sum = 0;
    for( auto digit : integer_token )
        sum = ( sum * 10 ) + digit - '0';   
    return sum;
}

constexpr static const char parenthesis_scope_name_string[] = "ParenthesisScope";
constexpr static const char factor_name_string[] = "Factor";
constexpr static const char sum_name_string[] = "Sum";

enum class RegexLiteralTerms {
    NaturalNumber = 0, 
    Identifier = 1
};

template< auto LiteralParameterType >
struct LiteralTerm
{
    static_assert( true, "Error::Unkown literal type -- no valid specification" );
    constexpr const static char regex[] = "";
    constexpr const static char name[] = "Nothing";
    constexpr const static auto term = ctpg::regex_term< regex >{ name };
    constexpr const static auto literal_type = LiteralParameterType;
};

template<>
struct LiteralTerm< RegexLiteralTerms::NaturalNumber >
{
    constexpr const static char regex[] = "[0-9][0-9]*";
    constexpr const static char name[] = "NaturalNumber";
    constexpr const static auto term = ctpg::regex_term< regex >{ name };
    constexpr const static auto literal_type = RegexLiteralTerms::NaturalNumber;
};

template<>
struct LiteralTerm< RegexLiteralTerms::Identifier >
{
    constexpr const static char regex[] = "[a-zA-Z\\_][a-zA-Z0-9\\_]*";
    constexpr const static char name[] = "Identifier";
    constexpr const static auto term = ctpg::regex_term< regex >{ name };
    constexpr const static auto literal_type = RegexLiteralTerms::Identifier;
};

template< auto TypeParameterConstant 
        = RegexLiteralTerms::NaturalNumber >
struct DefaultTypes {
    using Type = size_t;
};

template<>
struct DefaultTypes< RegexLiteralTerms::Identifier > {
    using Type = std::string;
};

template< 
        auto TypeParameterConstant = RegexLiteralTerms::NaturalNumber, 
        typename CPlusPlusTypeParameterConstant 
                = DefaultTypes< TypeParameterConstant >::Type 
    >
struct Converter {
    constexpr const static auto converter = to_size_t;
};

template< 
        typename OperhandTypeParameterType, 
        auto ConvertToTypeConstantParameter = Converter< RegexLiteralTerms::NaturalNumber >::converter, 
        typename LiteralRegexTermParameterConstant = LiteralTerm< RegexLiteralTerms::NaturalNumber >,
        typename IdentifierRegexTermParameterConstant = LiteralTerm< RegexLiteralTerms::Identifier >, 
        auto FactorNameParameterConstant = parenthesis_scope_name_string, 
        auto SumNameParameterConstant = factor_name_string, 
        auto ParanthesisScopeParameterConstant = sum_name_string, 
        auto PlusCharacterParameterConstant = '+', 
        auto MinusCharacterParameterConstant = '-', 
        auto MultiplyCharacterParameterConstant = '*', 
        auto DivideCharacterParameterConstant = '/', 
        auto LeftParanethesisCharacterParameterConstant = '(', 
        auto RightParanethesisCharacterParameterConstant = ')' 
    >
struct ConstantExpression
{

    constexpr static auto factor = ctpg::nterm< OperhandTypeParameterType >{ FactorNameParameterConstant };
    constexpr static auto sum = ctpg::nterm< OperhandTypeParameterType >{ SumNameParameterConstant };
    constexpr static auto parenthesis_scope = ctpg::nterm< OperhandTypeParameterType >{ ParanthesisScopeParameterConstant };

    constexpr static auto nterms = ctpg::nterms( 
            factor, sum, parenthesis_scope 
        );

    constexpr static auto plus_term = ctpg::char_term{ 
            PlusCharacterParameterConstant, 
            1, 
            ctpg::associativity::ltor 
        };
    constexpr static auto minus_term = ctpg::char_term{ 
            MinusCharacterParameterConstant, 
            1, 
            ctpg::associativity::ltor 
        };
    constexpr static auto multiply_term = ctpg::char_term{ 
            MultiplyCharacterParameterConstant, 
            2, 
            ctpg::associativity::ltor 
        };
    constexpr static auto divide_term = ctpg::char_term{ 
            DivideCharacterParameterConstant, 
            2, 
            ctpg::associativity::ltor 
        };
    constexpr static auto left_parenthesis_term = ctpg::char_term{ 
            LeftParanethesisCharacterParameterConstant, 
            3, 
            ctpg::associativity::ltor 
        };
    constexpr static auto right_parenthesis_term = ctpg::char_term{ 
            RightParanethesisCharacterParameterConstant, 
            3, 
            ctpg::associativity::ltor 
        };

    constexpr static auto terms = ctpg::terms(
            plus_term, 
            minus_term, 
            multiply_term, 
            divide_term, 
            left_parenthesis_term, 
            right_parenthesis_term 
        );

    constexpr static auto rules = ctpg::rules( 
            factor( LiteralRegexTermParameterConstant::term ) >= ConvertToTypeConstantParameter, 
            factor( factor, multiply_term, IdentifierRegexTermParameterConstant::term ) 
                >= []( size_t current_factor, auto, const auto& next_token ) {  
                        return current_factor * ConvertToTypeConstantParameter( next_token ); 
                    }, 
            factor( factor, divide_term, IdentifierRegexTermParameterConstant::term ) 
                >= []( size_t current_factor, auto, const auto& next_token ) {
                        return current_factor / ConvertToTypeConstantParameter( next_token ); 
                    },
            parenthesis_scope( left_parenthesis_term, factor, right_parenthesis_term )
                    >= [] ( auto, auto factor, auto ) { return factor; }, 
            factor( parenthesis_scope ) >= []( auto parenthesis_scope ) { return parenthesis_scope; }, 
            factor( factor, multiply_term, parenthesis_scope ) 
                >= []( auto factor, auto, auto parenthesis_scope ) { 
                        return factor * parenthesis_scope; 
                    }, 
            factor( factor, divide_term, parenthesis_scope ) 
                >= []( auto factor, auto, auto parenthesis_scope ) { 
                        return factor / parenthesis_scope; 
                    }, 
            factor( sum ) >= []( auto sum ) { return sum; }, 
            sum( factor, plus_term, factor ) 
                >= []( auto current_sum, auto, const auto& next_token ) {
                        return  current_sum + next_token; 
                    }, 
            sum( factor, minus_term, factor ) 
                >= []( auto current_sum, auto, const auto& next_token ) {
                        return current_sum - next_token; 
                    } 
        );
};
