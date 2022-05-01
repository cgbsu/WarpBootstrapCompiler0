#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_HPP

namespace Warp::Utilities
{
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
    struct AutoVariant;

    template< auto VisitorParameterConstant, typename... ParameterTypes >
    constexpr static auto visit( 
            const AutoVariant< ParameterTypes... >& AutoVariant 
        ) noexcept;

    template< typename... ParameterTypes >
    struct AutoVariant
    {
        template< typename ParameterType >
        constexpr static size_t type_index = FindTypeIndex< 
                        0, 
                        ParameterType, 
                        ParameterTypes... 
                    >::type_index;

        template< typename AlternativeParameterType, typename... InitializersParameterTypes >
        constexpr AutoVariant( std::in_place_type_t< AlternativeParameterType >, InitializersParameterTypes&&... initializers ) noexcept
                : data( static_cast< void* >( new AlternativeParameterType( 
                        std::forward< InitializersParameterTypes >( initializers )... ) 
                    ) ), 
                alternative_index( type_index< AlternativeParameterType > ) {}
        constexpr ~AutoVariant() noexcept
        {
            visit< []( auto* data_ ) {
                delete static_cast< decltype( data_ ) >( data_ );
                return nullptr; }>( *this );
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
    constexpr bool holds_alternative( auto AutoVariant ) {
        return decltype( AutoVariant )::template type_index< AlternativeType >() == AutoVariant.index();
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
                const AutoVariant< ParameterTypes... >& AutoVariant 
            ) noexcept : result( 
            ( IndexParameterConstant == AutoVariant.index() ) ? 
                VisitorParameterConstant( static_cast< 
                        IndexToType< 
                                IndexParameterConstant, 
                                0, 
                                ParameterTypes... >::Type* 
                    >( AutoVariant.get_data() ) ) 
            : 
            VisitImplementation< 
                    ReturnParameterType, 
                    IndexParameterConstant + 1, 
                    MaximumParameterConstant, 
                    VisitorParameterConstant, 
                    ParameterTypes... 
                >( AutoVariant ).result ) {}
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
                const AutoVariant< ParameterTypes... >& AutoVariant 
            ) noexcept : result( VisitorParameterConstant( static_cast< 
                        IndexToType< 
                                MaximumParameterConstant, 
                                0, 
                                ParameterTypes... >::Type* 
                    >( AutoVariant.get_data() ) )
                ) {}
    };

    template< auto VisitorParameterConstant, typename... ParameterTypes >
    constexpr static auto visit( 
            const AutoVariant< ParameterTypes... >& AutoVariant 
        ) noexcept
    {
        using FirstAlternativeType = typename IndexToType< 0, 0, ParameterTypes... >::Type;
        using ReturnType = decltype( VisitorParameterConstant( 
                new FirstAlternativeType{} ) 
            );
        return VisitImplementation< 
                ReturnType, 
                0, 
                sizeof...( ParameterTypes ) - 1, 
                VisitorParameterConstant, 
                ParameterTypes... 
            >( AutoVariant ).result;
    }
    template< typename ParameterType >
    struct NotSoUniquePointer
    {
        using Type = ParameterType;
        template< typename... InitializerParameterTypes >
        constexpr explicit NotSoUniquePointer( InitializerParameterTypes&&... initalizers ) noexcept 
                : pointer( new Type( initalizers... ) ) {}
        constexpr NotSoUniquePointer( const NotSoUniquePointer< Type >& other ) noexcept 
                : pointer( other.pointer ) {
            // static_cast< NotSoUniquePointer< Type >& >( 
                ( ( NotSoUniquePointer< Type >& ) other ).pointer = nullptr;
        }
        constexpr NotSoUniquePointer( NotSoUniquePointer< Type >&& other ) noexcept 
                : pointer( other.pointer ) {
            other.pointer = nullptr;
        }
        constexpr ~NotSoUniquePointer() {
            delete pointer;
        }
        
    protected: 
            Type* pointer;
    };
}

    /*
    #include <cxxabi.h>
    auto type_name( auto data ) {
        int status;
        return abi::__cxa_demangle( typeid( data ).name(), 0, 0, &status );
    }


    int main( int argc, char** args )
    {
        auto x = AutoVariant< int, bool, float >( std::in_place_type_t< float >{}, 42.5f );
        std::cout << visit< []( auto p ) { 
                std::cout << type_name( p ) 
                        << " val " 
                        << *p 
                        << "\n"; 
                return static_cast< float >( *p * *p ); 
            } >( x );
        return 0;
    }

    consteval auto test( auto val )
    {
        auto x = AutoVariant< int, bool, float, char, double, size_t >( std::in_place_type_t< decltype( val ) >{}, val );
        return visit< []( auto p ) { 
                // std::cout << type_name( p ) 
                //         << " val " 
                //         << *p 
                //         << "\n"; 
                return static_cast< float >( *p * *p ); 
            } >( x );
    }

    int main( int argc, char** args )
    {
        std::cout << "result: " << test( 4.2 );
        return 0;
    }

    */
#endif // WARP_BOOTSTRAP_COMPILER_HEADER_UTILITES_HPP