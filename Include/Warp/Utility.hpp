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

#include <iostream>
#include <type_traits>
#include <tuple>
#include <functional>

template< 
        size_t IndexParameterConstant, 
        typename QueryParameterType, 
        typename CurrentParameterType, 
        typename... ParameterTypes 
    >
struct FindTypeIndex
{
    constexpr static const size_t type_index = FindTypeIndex< 
            IndexParameterConstant + 1, 
            QueryParameterType, 
            ParameterTypes... 
        >::type_index;
};

template< 
        size_t IndexParameterConstant, 
        typename QueryParameterType, 
        typename... ParameterTypes 
    >
struct FindTypeIndex< 
        IndexParameterConstant, 
        QueryParameterType, 
        QueryParameterType, // Same type twice in a row means we found it! //
        ParameterTypes... 
    > {
    constexpr static const size_t type_index = IndexParameterConstant;
};

template< 
        size_t IndexParameterConstant, 
        size_t CurrentIndexParameterConstant, 
        typename CurrentParameterType, 
        typename... ParameterTypes 
    >
struct IndexToType
{
    using Type = typename IndexToType< 
            IndexParameterConstant, 
            CurrentIndexParameterConstant + 1, 
            ParameterTypes ...
        >::Type;
};

template< typename AnyParameterType >
struct AnyParameter {
    using Type = AnyParameterType;
};

template< typename AffirmativeParmaterType >
struct NullNotType
{
    const AffirmativeParmaterType* pointer;
    constexpr NullNotType( const AffirmativeParmaterType* pointer ) : pointer( pointer ) {}
    constexpr NullNotType( auto ) : pointer( nullptr ) {}
};

template< typename CurrentParameterType, typename... ParameterTypes >
struct LightTuple : public LightTuple< ParameterTypes... >
{
    using Type = CurrentParameterType;
    using BaseType = LightTuple< ParameterTypes... >;
    using ThisType = LightTuple< Type, ParameterTypes... >;

    const Type* data;

    constexpr LightTuple( 
            const Type* data, 
            const ParameterTypes*... other_datas 
        ) : BaseType( other_datas... ), data( data ) {}

    /*template< typename SuperParameterType >
    constexpr explicit LightTuple( 
            const LightTuple< SuperParameterType, Type, ParameterTypes... >& other 
        ) : ThisType( static_cast< const ThisType& >( other ) ) {}*/

    constexpr LightTuple( 
            const Type* data, 
            const BaseType& other 
        ) : BaseType( other ), data( data ) {}

    template< typename... ConstructorParameterTypes >
    constexpr LightTuple( 
            NullNotType< CurrentParameterType > data
        ) : BaseType( data ), data( data.pointer ) {}

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
    const Type* data;
    using ThisType = LightTuple< Type >;

    constexpr LightTuple( 
            const Type* data 
        ) : data( data ) {}
    
    template< typename... ConstructorParameterTypes >
    constexpr LightTuple( 
            NullNotType< CurrentParameterType > data
        ) : data( data.pointer ) {}

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
        -> IndexToType< TargetParameterConstant, 0, ParameterTypes... >::Type {
    return nullptr;
}


template< typename ReturnParameterType >
struct FunctionReturnType
{
    using ReturnType = ReturnParameterType;
    template< typename... ParameterTypes >
    constexpr FunctionReturnType( ReturnType( *function )( ParameterTypes... ) ) {}
};

template< 
        typename... TupleTypes 
    >
constexpr auto remove_element( 
            const LightTuple< TupleTypes... >& tuple 
        ) -> const LightTuple< TupleTypes... >::BaseType& {
    static_cast< const decltype( tuple )::BaseType& >( tuple );
}

template< typename ParameterType >
ParameterType& refer_to( ParameterType object ) {
    return object;
}

template< typename ParameterType, typename... ParameterTypes >
struct TakeOne {
    using Type = ParameterType;
    using NextType = TakeOne< ParameterTypes... >;
};

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
                >::ReturnTuple< 
                        NewTupleParameterTypes..., 
                        decltype( OperationParameterConstant( 
                                CurrentParameterType{} 
                            ) ) 
                    >;
        using ReturnTupleType = InjectionType::ReturnTupleType;
        template< template< auto, typename... > typename RecievingTemplateParameterTemplate >
        using InjectTypes = typename InjectionType::InjectTypes< RecievingTemplateParameterTemplate >;
    };
    template< template< auto, typename... > typename RecievingTemplateParameterTemplate >
    using InjectTupleTypes = typename ReturnTuple<>::InjectTypes< RecievingTemplateParameterTemplate >;
    using ReturnType = ReturnTuple<>::ReturnTupleType;
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
                        NewTupleParameterTypes..., 
                        decltype( OperationParameterConstant( 
                                CurrentParameterType{} 
                            ) ) 
                    >;
        template< template< auto, typename... > typename RecievingTemplateParameterTemplate >
        using InjectTypes = RecievingTemplateParameterTemplate< OperationParameterConstant, NewTupleParameterTypes... >;
    };
};

template< typename ParameterType >
struct AddPointer {
    using Type = ParameterType*;
};

template< 
        auto OperationParameterConstant, 
        typename CurrentParameterType, 
        typename... TupleParameterTypes 
    >
struct Apply
{
    using TupleType = LightTuple< TupleParameterTypes... >;
    using NextTupleMakerType = TupleFromOperation< 
            OperationParameterConstant, 
            CurrentParameterType, 
            TupleParameterTypes... 
        >;
    using NextApplyType = Apply< OperationParameterConstant, TupleParameterTypes... >;/
    using NextTupleType = NextTupleMakerType::ReturnType;
    const NextTupleType result;
    constexpr Apply( const TupleType& tuple ) : result( 
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
    using ResultType = LightTuple< CurrentParameterType >;
    const ResultType result;
    constexpr Apply( TupleType tuple ) : result( 
            LightTuple( OperationParameterConstant( tuple.data ) ) ) {}
    // constexpr Apply( const TupleType& tuple ) : result( 
            // LightTuple( OperationParameterConstant( tuple.data ) ) ) {}
};

template< 
        auto OperationParameterConstant, 
        typename... TupleParameterTypes 
    >
auto apply( const LightTuple< TupleParameterTypes... >& tuple ) {
    return Apply< OperationParameterConstant, TupleParameterTypes... >( tuple );
}

template< typename... ParameterTypes >
struct AutoVariant
{
    template< typename ParameterType >
    constexpr static size_t type_index = FindTypeIndex< 
                    0, 
                    ParameterType, 
                    ParameterTypes... 
                >::type_index;
    constexpr AutoVariant( auto data_ ) noexcept
            : data( static_cast< const void* >( new decltype( data_ )( data_ ) ) ), 
            alternative_index( type_index< decltype( data_ ) >  ) {}

    constexpr ~AutoVariant() noexcept {
        delete static_cast< const int* >( data );
    }

    template< typename ParameterType >
    constexpr bool holds_alternative() const noexcept {
        return FindTypeIndex< 0, ParameterType, ParameterTypes... >::type_index == alternative_index;
    }

    constexpr auto get_data() const noexcept {
        return static_cast< typename IndexToType< alternative_index, 
                0, ParameterTypes... >::Type* >( data );
    }

    const void* data;
    const size_t alternative_index;
};

template< typename ParameterType >
struct HoldsAlternative
{
    const bool value;
    const ParameterType* data;
    template< typename... ParameterTypes >
    constexpr HoldsAlternative( const AutoVariant< ParameterTypes... >& variant ) 
            : value( FindTypeIndex< 0, ParameterType, ParameterTypes... >::type_index == variant.alternative_index ) {} 
};
