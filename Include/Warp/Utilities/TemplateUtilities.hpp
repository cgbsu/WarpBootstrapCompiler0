#include <Warp/Common.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_TEMPLATE_UTILITIES_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_TEMPLATE_UTILITIES_HPP

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

    template< typename FirstParameterType, typename... ParameterTypes >
    struct TakeFirstType {
        using Type = FirstParameterType;
        // using NextType = TakeFirstType< ParameterTypes... >;
    };

    template< typename FirstParameterType >
    struct TakeFirstType< FirstParameterType > {
        using Type = FirstParameterType;
    };

    template< 
            typename QueryParameterType, 
            typename AlternativeParameterType, 
            bool EnableParameterConstant 
        >
    struct EnableInject {
        using Type = AlternativeParameterType;
    };

    template< 
            typename QueryParameterType, 
            typename AlternativeParameterType 
        >
    struct EnableInject <
                QueryParameterType, 
                AlternativeParameterType, 
                true 
            > {
        using Type = QueryParameterType;
    };

    template< 
            template< auto > typename ToInjectIntoParameterType, 
            auto ValueParameterConstant, 
            auto AlternativeValueParameterConstant, 
            bool EnableParameterConstant 
        >
    struct EnableInjectValue {
        constexpr const static auto value = AlternativeValueParameterConstant;
    };

    template< 
            template< auto > typename ToInjectIntoParameterType, 
            auto ValueParameterConstant, 
            auto AlternativeValueParameterConstant 
        >
    struct EnableInjectValue< 
                ToInjectIntoParameterType, 
                ValueParameterConstant, 
                AlternativeValueParameterConstant, 
                false  
            > {
        constexpr const static auto value = ToInjectIntoParameterType< ValueParameterConstant >::value;
    };

    // template< 
    //         auto ValueParameterConstant, 
    //         auto AlternativeValueParameterConstant, 
    //         bool EnableParameterConstant = false 
    //     >
    // struct EnableValue {
    //     constexpr const static auto value = AlternativeValueParameterConstant;
    // };

    // template< 
    //         auto ValueParameterConstant, 
    //         auto AlternativeValueParameterConstant, 
    //     >
    // struct EnableValue< 
    //             ValueParameterConstant, 
    //             AlternativeValueParameterConstant, 
    //             true 
    //         > {
    //     constexpr const static auto value = ValueParameterConstant;
    // };

    template< 
            auto OperationParameterConstant, 
            auto LeftOperhandParameterConstant, 
            auto RightOperhandParameterConstant, 
            bool EnableParameterConstant,  
            auto NoOperationParameterConstant 
        >
    struct EnableOperation {
        constexpr const static auto value = NoOperationParameterConstant;
    };


    template< 
            auto OperationParameterConstant, 
            auto LeftOperhandParameterConstant, 
            auto RightOperhandParameterConstant, 
            auto NoOperationParameterConstant 
        >
    struct EnableOperation< 
            OperationParameterConstant, 
            LeftOperhandParameterConstant, 
            RightOperhandParameterConstant, 
            true, 
            NoOperationParameterConstant 
        > 
    {
        constexpr const static auto value = OperationParameterConstant( 
                LeftOperhandParameterConstant, 
                RightOperhandParameterConstant 
            );
    };



    template< typename UncleanParmaeterType >
    using CleanType = std::decay_t< typename std::remove_reference< 
            typename std::remove_pointer< UncleanParmaeterType >::type 
                >::type >;

    template< auto UncleanParmaeterConstant >
    using CleanTypeOfValueType = CleanType< decltype( UncleanParmaeterConstant ) >;


    template< typename QueryParameterType, typename... ParameterTypes >
    struct SameAsToFirstType 
    {
        constexpr const static bool value = std::is_same< 
                QueryParameterType, 
                typename TakeFirstType< ParameterTypes...  >::Type 
            >::value;
    };

    template< typename QueryParameterType >
    struct SameAsToFirstType< QueryParameterType > {
        //static_assert( false, "YOU SHOULDENT BE HERE" );
        constexpr const static bool value = false;        
    };

    template< 
            auto FirstValueParameterConstant, 
            auto SecondValueParameterConstant, 
            bool SentryParameterConstant 
        >
    struct ChooseType {
        constexpr const static auto value = SecondValueParameterConstant;
    };

    template< 
            auto FirstValueParameterConstant, 
            auto SecondValueParameterConstant 
        >
    struct ChooseType <
                FirstValueParameterConstant, 
                SecondValueParameterConstant, 
                true 
            > {
        constexpr const static auto value = FirstValueParameterConstant;
    };

    template< 
            size_t IndexParameterConstant, 
            typename QueryParameterType, 
            typename CurrentParameterType, 
            typename... ParameterTypes 
        >
    struct FindTypeIndex 
    {
        constexpr static const bool has_type = std::is_same< QueryParameterType, CurrentParameterType >::value;
        constexpr static const size_t type_index = ChooseType< 
                IndexParameterConstant, 
                FindTypeIndex< 
                        IndexParameterConstant + 1, 
                        QueryParameterType, 
                        ParameterTypes... 
                    >::type_index, 
                has_type 
            >::value;
    };

    template< 
            size_t IndexParameterConstant, 
            typename QueryParameterType, 
            typename... ParameterTypes 
        >
    struct FindTypeIndex< 
                IndexParameterConstant, 
                QueryParameterType, 
                QueryParameterType, 
                ParameterTypes... 
            > {
        constexpr static const bool has_type = std::is_same< QueryParameterType, QueryParameterType >::value;
        constexpr static const size_t type_index = IndexParameterConstant;
    };

    template< 
            size_t IndexParameterConstant, 
            typename QueryParameterType 
        >
    struct FindTypeIndex< 
                IndexParameterConstant, 
                QueryParameterType, 
                QueryParameterType
            > {
        constexpr static const bool has_type = true;
        constexpr static const size_t type_index = IndexParameterConstant;
    };

    template< 
            size_t IndexParameterConstant, 
            typename QueryParameterType, 
            typename CurrentParameterType 
        >
    struct FindTypeIndex< 
            IndexParameterConstant, 
            QueryParameterType, 
            CurrentParameterType
        >
    {
        static_assert( std::is_same< QueryParameterType, CurrentParameterType >::value, "Type not in type list\n" );
        constexpr static const bool has_type = std::is_same< QueryParameterType, CurrentParameterType >::value;
        constexpr static const size_t type_index = IndexParameterConstant;
    };

    template< 
            size_t IndexParameterConstant, 
            typename QueryParameterType, 
            typename... ParameterTypes 
        > requires( sizeof...( ParameterTypes ) > 0 )
    struct FindTypeIndexDecay
    {
        constexpr static const size_t type_index = FindTypeIndex< 
                IndexParameterConstant, 
                CleanType< std::decay_t< QueryParameterType > >, 
                CleanType< std::decay_t< ParameterTypes > >... 
            >::type_index;
    };

    template< 
            size_t IndexParameterConstant, 
            typename QueryParameterType, 
            typename CurrentParameterType 
        > requires( std::is_same< QueryParameterType, CurrentParameterType >::value == true )
    struct FindTypeIndexDecay< 
                IndexParameterConstant, 
                QueryParameterType, 
                CurrentParameterType 
            > {
        constexpr static const size_t type_index = IndexParameterConstant;
    };

    template< 
            size_t IndexParameterConstant, 
            size_t CurrentIndexParameterConstant, 
            typename CurrentParameterType, 
            typename... ParameterTypes 
        >
    struct IndexToTypeImplementation 
    {
        using NextType = typename IndexToTypeImplementation< 
                IndexParameterConstant, 
                CurrentIndexParameterConstant + 1, 
                ParameterTypes ...
            >::Type;
        using Type = EnableInject< CurrentParameterType, NextType, IndexParameterConstant == CurrentIndexParameterConstant >::Type;
    };

    template< 
            size_t IndexParameterConstant, 
            size_t CurrentParameterConstant, 
            typename CurrentParameterType 
        >
    struct IndexToTypeImplementation< 
                IndexParameterConstant, 
                CurrentParameterConstant, 
                CurrentParameterType 
            > {
        using Type = CurrentParameterType;
    };

    template< 
            size_t IndexParameterConstant, 
            size_t StartIndexParameter, 
            typename... ParameterTypes 
        >
    struct IndexToType
    {
        using Type = typename IndexToTypeImplementation< 
                IndexParameterConstant, 
                StartIndexParameter, 
                ParameterTypes ...
            >::Type;
    };

    template< typename ReturnParameterType >
    struct FunctionReturnType
    {
        using ReturnType = ReturnParameterType;
        template< typename... ParameterTypes >
        constexpr FunctionReturnType( ReturnType( *function )( ParameterTypes... ) ) noexcept {}
    };

    template< typename ParameterType >
    ParameterType& refer_to( ParameterType object ) {
        return object;
    }

    template< typename ParameterType, typename... ParameterTypes >
    struct TakeOne {
        using Type = ParameterType;
        using NextType = TakeOne< ParameterTypes... >;
    };

    template< typename ParameterType >
    struct AddPointer {
        using Type = ParameterType*;
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

    template< auto QueryParameterConstant, auto CurrentParameterConstant, auto... ParameterConstants >
    struct HasConstant {
        constexpr static const bool value = HasConstant< QueryParameterConstant, ParameterConstants... >::value;
    };

    template< 
            auto QueryParameterConstant, 
            auto... ParameterConstants
        >
    struct HasConstant< 
                QueryParameterConstant, 
                QueryParameterConstant, 
                ParameterConstants... 
            > {
        constexpr static const bool value = true;
    };

    template< auto QueryParameterConstant >
    struct HasConstant< 
                QueryParameterConstant, 
                QueryParameterConstant 
            > {
        constexpr static const bool value = true;
    };

    template< 
            auto QueryParameterConstant, 
            auto CurrentParameterConstant 
        >
    struct HasConstant< 
            QueryParameterConstant, 
            CurrentParameterConstant 
        > 
    {
        constexpr static const bool value = EnableOperation< 
                []( auto left, auto right ){ return left == right; }, 
                QueryParameterConstant, 
                CurrentParameterConstant, 
                false, 
                std::is_same< 
                        //CleanTypeOfValueType< 
                        decltype( QueryParameterConstant ), // >, 
                        //CleanTypeOfValueType< 
                        decltype( CurrentParameterConstant ) // > 
                    >::value 
            >::value;
    };

}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_TEMPLATE_UTILITIES_HPP
