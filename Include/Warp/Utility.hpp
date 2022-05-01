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
#include <string>
#include <type_traits>
#include <utility>



template< typename UncleanParmaeterType >
using CleanType = typename std::remove_reference< 
        typename std::remove_pointer< UncleanParmaeterType >::type 
            >::type;

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

template< 
        size_t IndexParameterConstant, 
        typename CurrentParameterType, 
        typename... ParameterTypes 
    >
struct IndexToType< 
        IndexParameterConstant, 
        IndexParameterConstant, 
        CurrentParameterType, 
        ParameterTypes... 
    > {
    using Type = CurrentParameterType;
};

template< typename AnyParameterType >
struct AnyParameter {
    using Type = AnyParameterType;
};

template< typename ParameterType >
using ToPointerType = std::remove_reference< ParameterType >::type*;

template< typename AffirmativeParameterType >
struct NullNotType
{
    using AffirmativeType = std::remove_reference< AffirmativeParameterType >::type;
    const AffirmativeType* pointer;
    constexpr NullNotType( const AffirmativeType* pointer ) noexcept : pointer( pointer ) {}
    constexpr NullNotType( auto ) noexcept : pointer( nullptr ) {}
};

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
            std::remove_reference< const Type >::type* data, 
            std::remove_reference< const ParameterTypes >::type*... other_datas 
        ) noexcept : BaseType( other_datas... ), data( data ) {}

    constexpr LightTuple( 
            const Type* data, 
            const BaseType& other 
        ) noexcept : BaseType( other ), data( data ) {}

    constexpr LightTuple( 
                std::remove_reference< const Type >::type* data
        ) noexcept : BaseType( data ), data( data ) {}

    constexpr LightTuple( 
                std::remove_reference< Type >::type* data
        ) noexcept : BaseType( data ), data( data ) {}

    template< typename AnyParameterType >
    constexpr LightTuple( 
                AnyParameterType* data
        ) noexcept : BaseType( data ), data( nullptr ) {}
        
    //data( NullifierType< const Type >( typename ToPointerType< AnyParameterType >{ data } ).pointer ) {}

    constexpr LightTuple( const ThisType& other ) = default;

    constexpr LightTuple( ThisType&& other ) = default;

    constexpr ThisType& operator=( const ThisType& other ) = default;

    constexpr ThisType& operator=( ThisType&& other ) = default;

    template< typename NewParameterType >
    using AppendType = LightTuple< CurrentParameterType, ParameterTypes..., NewParameterType >;

    template< typename PointerParameterType >
    using NullifierType = NullNotType< typename std::remove_reference< PointerParameterType >::type >;
};

template< typename CurrentParameterType >
struct LightTuple< CurrentParameterType > //typename std::remove_reference< CurrentParameterType >::type >
{
    using Type = CurrentParameterType;//std::remove_reference< CurrentParameterType >::type;
    std::remove_reference< const Type >::type* data;
    using ThisType = LightTuple< Type >;

    // constexpr LightTuple( 
    //         const Type& data 
    //     ) noexcept : data( &data ) {}

    constexpr LightTuple( 
            std::remove_reference< Type >::type* data 
        ) noexcept : data( data ) {}

    constexpr LightTuple( 
            std::remove_reference< const Type >::type* data 
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
        noexcept -> IndexToType< TargetParameterConstant, 0, ParameterTypes... >::Type {
    return nullptr;
}


template< typename ReturnParameterType >
struct FunctionReturnType
{
    using ReturnType = ReturnParameterType;
    template< typename... ParameterTypes >
    constexpr FunctionReturnType( ReturnType( *function )( ParameterTypes... ) ) noexcept {}
};

template< 
        typename... TupleTypes 
    >
constexpr auto remove_element( 
            const LightTuple< TupleTypes... >& tuple 
        ) noexcept -> const LightTuple< TupleTypes... >::BaseType& {
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
                        CleanType< NewTupleParameterTypes >..., 
                        CleanType< decltype( 
                                OperationParameterConstant( new CurrentParameterType{} ) 
                            ) >
                    >;
        using ReturnTupleType = InjectionType::ReturnTupleType;
        template< template< auto, typename... > typename RecievingTemplateParameterTemplate >
        using InjectTypes = typename InjectionType::InjectTypes< 
                RecievingTemplateParameterTemplate 
            >;
    };
    template< template< auto, typename... > typename RecievingTemplateParameterTemplate >
    using InjectTupleTypes = typename ReturnTuple<>::InjectTypes< 
            RecievingTemplateParameterTemplate 
        >;
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
    using TupleType = LightTuple< CurrentParameterType, TupleParameterTypes... >;
    using NextTupleMakerType = TupleFromOperation< 
            OperationParameterConstant, 
            CurrentParameterType, 
            TupleParameterTypes... 
        >;
    using NextApplyType = Apply< OperationParameterConstant, TupleParameterTypes... >;
    using NextTupleType = NextTupleMakerType::ReturnType;
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

template< typename... ParameterTypes >
struct Variant
{
    template< typename ParameterType >
    constexpr static size_t type_index = FindTypeIndex< 
                    0, 
                    ParameterType, 
                    ParameterTypes... 
                >::type_index;

    constexpr Variant( auto data_ ) noexcept
            : data( static_cast< void* >( new decltype( data_ )( data_ ) ) ), 
            alternative_index( type_index< decltype( data_ ) > ) {}
    constexpr ~Variant() noexcept {
        delete static_cast< const int* >( data );
    }
    constexpr const size_t index() const noexcept {
        return alternative_index;
    }
    constexpr void* get_data() const noexcept {
        return data;
    }
    protected: 
        void* data;
        const size_t alternative_index;
};


template< typename AlternativeType >
constexpr bool holds_alternative( auto variant ) {
    return decltype( variant )::template type_index< AlternativeType >() == variant.index();
}

template< 
        typename ReturnParameterType, 
        size_t IndexParameterConstant, 
        size_t MaximumParameterConstant, 
        auto VisitorParameterConstant, 
        typename... ParameterTypes 
    >
struct VisitImplementation
{
    ReturnParameterType result;
    constexpr VisitImplementation( 
            const Variant< ParameterTypes... >& variant 
        ) noexcept : result( 
        ( IndexParameterConstant == variant.index() ) ? 
            VisitorParameterConstant( static_cast< 
                    IndexToType< 
                            IndexParameterConstant, 
                            0, 
                            ParameterTypes... >::Type* 
                >( variant.get_data() ) ) 
         : 
        VisitImplementation< 
                ReturnParameterType, 
                IndexParameterConstant + 1, 
                MaximumParameterConstant, 
                VisitorParameterConstant, 
                ParameterTypes... 
            >( variant ).result ) {}
};

template< 
        typename ReturnParameterType, 
        size_t MaximumParameterConstant, 
        auto VisitorParameterConstant, 
        typename... ParameterTypes 
    >
struct VisitImplementation< 
        ReturnParameterType, 
        MaximumParameterConstant, 
        MaximumParameterConstant, 
        VisitorParameterConstant, 
        ParameterTypes... 
    >
{
    ReturnParameterType result;
    constexpr VisitImplementation( 
            const Variant< ParameterTypes... >& variant 
        ) noexcept : result( VisitorParameterConstant( static_cast< 
                    IndexToType< 
                            MaximumParameterConstant, 
                            0, 
                            ParameterTypes... >::Type* 
                >( variant.get_data() ) )
            ) {}
};

template< auto VisitorParameterConstant, typename... ParameterTypes >
constexpr static auto visit( 
        const Variant< ParameterTypes... >& variant 
    ) noexcept {
    using ReturnType = decltype( VisitorParameterConstant( 
            typename IndexToType< 0, 0, ParameterTypes... >::Type{} ) 
        );
    return VisitImplementation< 
            ReturnType, 
            0, 
            sizeof...( ParameterTypes ) - 1, 
            VisitorParameterConstant, 
            ParameterTypes... 
        >( variant ).result;
}



#include <cxxabi.h>
auto type_name( auto data ) {
    int status;
    return abi::__cxa_demangle( typeid( data ).name(), 0, 0, &status );
}


int main( int argc, char** args )
{
    auto x = Variant< int, bool, float >( 42.f );
    visit< []( auto p ) { std::cout << p << "\n"; return 2; } >( x );
    return 0;
}
