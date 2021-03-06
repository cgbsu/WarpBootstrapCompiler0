#include <Warp/Utilities/TemplateUtilities.hpp>

///////////////////////////////////////////////////////////////////////////////////
// I want to thank Matthew Godbolt https://godbolt.org/ for providing a ///////////
// good enviornment I could test this out in, isolated from the rest of the code //
// (which was on fire at the time), and get quick feedback. Compiler Explorere ////
// is awesome!! ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_LIGHT_TUPLE_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_LIGHT_TUPLE_HPP

namespace Warp::Utilities
{
    template< typename CurrentParameterType, typename... ParameterTypes >
    struct LightTuple : public LightTuple< typename std::remove_reference< ParameterTypes >::type... >
    {
        using Type = CurrentParameterType;
        using BaseType = LightTuple< ParameterTypes... >;
        using ThisType = LightTuple< Type, ParameterTypes... >;

        const Type* data;

        constexpr LightTuple( 
                const Type& data, 
                const ParameterTypes&... other_datas 
            ) noexcept : BaseType( other_datas... ), data( &data ) {}


        constexpr LightTuple( 
                typename std::remove_reference< const Type >::type* data, 
                typename std::remove_reference< const ParameterTypes >::type*... other_datas 
            ) noexcept : BaseType( other_datas... ), data( data ) {}

        constexpr LightTuple( 
                const Type* data, 
                const BaseType& other 
            ) noexcept : BaseType( other ), data( data ) {}

        constexpr LightTuple( 
                    typename std::remove_reference< const Type >::type* data
            ) noexcept : BaseType( data ), data( data ) {}

        constexpr LightTuple( 
                    typename std::remove_reference< Type >::type* data
            ) noexcept : BaseType( data ), data( data ) {}

        template< typename AnyParameterType >
        constexpr LightTuple( 
                    AnyParameterType* data
            ) noexcept : BaseType( data ), data( nullptr ) {}
        
        constexpr LightTuple( const ThisType& other ) = default;

        constexpr LightTuple( ThisType&& other ) = default;

        constexpr ThisType& operator=( const ThisType& other ) = default;

        constexpr ThisType& operator=( ThisType&& other ) = default;

        template< typename NewParameterType >
        using AppendType = LightTuple< CurrentParameterType, ParameterTypes..., NewParameterType >;

    };

    template< typename CurrentParameterType >
    struct LightTuple< CurrentParameterType >
    {
        using Type = CurrentParameterType;
        typename std::remove_reference< const Type >::type* data;
        using ThisType = LightTuple< Type >;

        constexpr LightTuple( 
                typename std::remove_reference< Type >::type* data 
            ) noexcept : data( data ) {}

        constexpr LightTuple( 
                typename std::remove_reference< const Type >::type* data 
            ) noexcept : data( data ) {}
        
    constexpr LightTuple( 
                auto
            ) noexcept : data( nullptr ) {}

        constexpr LightTuple( const ThisType& other ) = default;

        constexpr LightTuple( ThisType&& other ) = default;

        constexpr ThisType& operator=( const ThisType& other ) = default;
        
        constexpr ThisType& operator=( ThisType&& other ) = default;

        template< typename NewParameterType >
        using AppendType = LightTuple< CurrentParameterType, NewParameterType >;
    };

    template< 
            size_t TargetParameterConstant, 
            typename... ParameterTypes 
        >
    constexpr auto tuple_element_type( const LightTuple< ParameterTypes... > tuple ) 
            noexcept -> typename IndexToType< TargetParameterConstant, 0, ParameterTypes... >::Type {
        return nullptr;
    }


    template< 
            typename... TupleTypes 
        >
    constexpr auto remove_element( 
                const LightTuple< TupleTypes... >& tuple 
            ) noexcept -> const typename LightTuple< TupleTypes... >::BaseType& {
        static_cast< const typename decltype( tuple )::BaseType& >( tuple );
    }
    template< 
            auto OperationParameterConstant,
            typename CurrentParameterType, 
            typename... TupleParameterTypes 
        >
    struct TupleFromOperation
    {
        template< typename... NewTupleParameterTypes >
        struct ReturnTuple
        {

            using InjectionType = typename TupleFromOperation< 
                            OperationParameterConstant, 
                            TupleParameterTypes... 
                    >::template ReturnTuple< 
                            CleanType< NewTupleParameterTypes >..., 
                            CleanType< decltype( 
                                    OperationParameterConstant( new CurrentParameterType{} ) 
                                ) >
                        >;
            using ReturnTupleType = typename InjectionType::ReturnTupleType;
            template< template< auto, typename... > typename RecievingTemplateParameterTemplate >
            using InjectTypes = typename InjectionType::template InjectTypes< 
                    RecievingTemplateParameterTemplate 
                >;
        };
        template< template< auto, typename... > typename RecievingTemplateParameterTemplate >
        using InjectTupleTypes = typename ReturnTuple<>::template InjectTypes< 
                RecievingTemplateParameterTemplate 
            >;
        using ReturnType = typename ReturnTuple<>::ReturnTupleType;
    };

    template< 
            auto OperationParameterConstant,
            typename CurrentParameterType 
        >
    struct TupleFromOperation< 
            OperationParameterConstant,
            CurrentParameterType 
        >
    {
        template< typename... NewTupleParameterTypes >
        struct ReturnTuple
        {
            using InjectionType = ReturnTuple< NewTupleParameterTypes... >;
            using ReturnTupleType = LightTuple< 
                            CleanType< NewTupleParameterTypes >..., 
                            CleanType< decltype( 
                                    OperationParameterConstant( new CurrentParameterType{} ) 
                                ) >
                        >;
            template< template< auto, typename... > typename RecievingTemplateParameterTemplate >
            using InjectTypes = RecievingTemplateParameterTemplate< 
                    OperationParameterConstant, 
                    NewTupleParameterTypes... 
                >;
        };
    };

    template< 
            auto OperationParameterConstant, 
            typename CurrentParameterType, 
            typename... TupleParameterTypes 
        >
    struct Apply
    {
        using TupleType = LightTuple< CurrentParameterType, TupleParameterTypes... >;
        using NextTupleMakerType = TupleFromOperation< 
                OperationParameterConstant, 
                CurrentParameterType, 
                TupleParameterTypes... 
            >;
        using NextApplyType = Apply< OperationParameterConstant, TupleParameterTypes... >;
        using NextTupleType = typename NextTupleMakerType::ReturnType;
        const NextTupleType result;
        constexpr Apply( const TupleType& tuple ) noexcept : result( 
                LightTuple( OperationParameterConstant( tuple.data ), 
                        NextApplyType( tuple ).result ) ) {}

    };

    template< 
            auto OperationParameterConstant, 
            typename CurrentParameterType 
        >
    struct Apply< 
            OperationParameterConstant, 
            CurrentParameterType 
        >
    {
        using TupleType = LightTuple< CurrentParameterType >;
        using ResultType = LightTuple< CleanType< decltype( 
                OperationParameterConstant( new CurrentParameterType ) 
            ) > >;
        const ResultType result;
        constexpr Apply( const TupleType& tuple ) noexcept : result( 
                ResultType( OperationParameterConstant( tuple.data ) ) ) {}
    };

    template< 
            auto OperationParameterConstant, 
            typename... TupleParameterTypes 
        >
    constexpr auto apply( const LightTuple< TupleParameterTypes... >& tuple ) noexcept {
        return Apply< OperationParameterConstant, TupleParameterTypes... >( tuple ).result;
    }
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_LIGHT_TUPLE_HPP
