#include <Warp/Utilities.hpp>
#include <ctpg.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_CTPG_UTILITIES_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_CTPG_UTILITIES_HPP

namespace Warp::Utilities
{

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
    };
#endif //WARP_BOOTSTRAP_COMPILER_HEADER_CTPG_UTILITIES_HPP