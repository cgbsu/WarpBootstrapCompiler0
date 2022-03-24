#include <iostream>
#include <functional>
#include <array>
#include <ctpg.hpp>

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
};

template< 
        auto ArrayParameterConstant, 
        auto IndexParameterConstant, 
        auto ArrayLengthParameterConstant, 
        auto... ElementParameterConstants 
    >
struct ToRawTemplateArrayImplementation
{
    using ResultType = ToRawTemplateArrayImplementation< 
            ArrayParameterConstant, 
            IndexParameterConstant + 1, 
            ArrayLengthParameterConstant, 
            ElementParameterConstants..., 
            ArrayParameterConstant[ IndexParameterConstant ] 
        >::ResultType;
};

template< 
        auto ArrayParameterConstant, 
        auto IndexParameterConstant, 
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
    using ResultType = ToRawTemplateArrayImplementation< 
            ArrayParameterConstant, 
            0, 
            // 2
            std::array< decltype( ArrayParameterConstant[ 0 ] ), sizeof( ArrayParameterConstant ) >{}.size() ) 
        >::ResultType;
};

// This function was "yoiked" directly from https://github.com/peter-winter/ctpg //

constexpr size_t to_size_t( std::string_view integer_token )
{
    size_t sum = 0;
    for( auto digit : integer_token )
        sum = ( sum * 10 ) + digit - '0';   
    return sum;
}


/*template< typename CanidateTypeParameter >
concept ConstantAlgebraicConcept = 
        requires( CanidateTypeParameter left, CanidateTypeParameter right ) 
        {
                { left + right } -> CanidateTypeParameter;
                { left - right } -> CanidateTypeParameter;
                { left * right } -> CanidateTypeParameter;
                { left / right } -> CanidateTypeParameter;
        };*/

constexpr static const char natural_number_regex_string[] = "[0-9][0-9]*";
constexpr static const char identifier_regex_string[] = "[a-zA-Z\\_][a-zA-Z0-9\\_]*";
constexpr static const char parenthesis_scope_name_string[] = "ParenthesisScope";
constexpr static const char identifier_name_string[] = "Identifier";
constexpr static const char natural_number_name_string[] = "NaturalNumber";
constexpr static const char factor_name_string[] = "Factor";
constexpr static const char sum_name_string[] = "Sum";

constexpr static auto natural_number_regex = ToRawTemplateArray< natural_number_regex_string >::ResultType{};
constexpr static auto identifier_regex = ToRawTemplateArray< identifier_regex_string >::ResultType{};
constexpr static auto parenthesis_scope_nterm_name = ToRawTemplateArray< parenthesis_scope_name_string >::ResultType{};
constexpr static auto identifier_term_name = std::array< const char, sizeof( identifier_name_string ) >{ identifier_name_string };  // = ToRawTemplateArray< identifier_name_string >::ResultType{};

using DefaultConstantExpressionConstantAlgebraicType = size_t;
constexpr static auto defualt_constant_expression_data_type_name = ToRawTemplateArray< natural_number_name_string >::ResultType{};
constexpr static auto defualt_constant_expression_literal_regex = natural_number_regex;
constexpr static auto defualt_constant_expression_identifier_regex = identifier_regex;

constexpr static auto defualt_constant_expression_factor_nterm_name = ToRawTemplateArray< factor_name_string >::ResultType{};
constexpr static auto defualt_constant_expression_sum_nterm_name = ToRawTemplateArray< sum_name_string >::ResultType{};
constexpr static auto defualt_constant_expression_parenthesis_scope_nterm_name = parenthesis_scope_nterm_name;
constexpr static auto defualt_constant_expression_identifier_term_name = identifier_term_name;



template< 
        auto ConvertToTypeConstantParameter, 
        typename OperhandTypeParameterType, 
        auto DataTypeNameParameterConstant, 
        auto PlusCharacterParameterConstant = '+', 
        auto MinusCharacterParameterConstant = '-', 
        auto MultiplyCharacterParameterConstant = '*', 
        auto DivideCharacterParameterConstant = '/', 
        auto LeftParanethesisCharacterParameterConstant = '(', 
        auto RightParanethesisCharacterParameterConstant = ')', 
        auto LiteralRegexParameterConstant 
                = defualt_constant_expression_literal_regex, 
        auto IdentifierRegexParameterConstant 
                = defualt_constant_expression_identifier_regex, 
        auto IdentifierNameParameterConstant 
                = defualt_constant_expression_identifier_term_name, 
        auto FactorNameParameterConstant 
                = defualt_constant_expression_factor_nterm_name, 
        auto SumNameParameterConstant 
                = defualt_constant_expression_sum_nterm_name, 
        auto ParanthesisScopeParameterConstant 
                = defualt_constant_expression_parenthesis_scope_nterm_name 
    >
struct ConstantExpression
{
    using DataType = OperhandTypeParameterType;
    constexpr static auto data_type_name = DataTypeNameParameterConstant;

    constexpr static const char literal_regex[ decltype( LiteralRegexParameterConstant )::size ] = { decltype( LiteralRegexParameterConstant )::array };
    constexpr static auto literal_term = ctpg::regex_term< literal_regex >{ data_type_name };

    constexpr static auto identifier_regex = IdentifierRegexParameterConstant;
    constexpr static auto identifier = ctpg::regex_term< identifier_regex >{ IdentifierNameParameterConstant };

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
        factor( literal_term ) >= ConvertToTypeConstantParameter, 
        factor( factor, multiply_term, literal_term ) 
            >= []( size_t current_factor, auto, const auto& next_token ) {  
                    return current_factor * ConvertToTypeConstantParameter( next_token ); 
                }, 
        factor( factor, divide_term, literal_term ) 
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
