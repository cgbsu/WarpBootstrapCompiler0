#include <Warp/Utilities/TemplateUtilities.hpp>

///////////////////////////////////////////////////////////////////////////////////
// I want to thank Matthew Godbolt https://godbolt.org/ for providing a ///////////
// good enviornment I could test this out in, isolated from the rest of the code //
// (which was on fire at the time), and get quick feedback. Compiler Explorere ////
// is awesome!! ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_AUTO_VARIANT_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_AUTO_VARIANT_HPP

namespace Warp::Utilities
{
    template< typename... ParameterTypes >
    struct AutoVariant;

    template< auto VisitorParameterConstant, typename... ParameterTypes >
    constexpr static auto visit( 
            const AutoVariant< ParameterTypes... >& variant 
        ) noexcept;

    template< typename... ParameterTypes >
    struct AutoVariant
    {
        using ThisType = AutoVariant< ParameterTypes... >;

        template< typename ParameterType >
        constexpr static size_t type_index = FindTypeIndexDecay< 
                        0, 
                        ParameterType, 
                        ParameterTypes... 
                    >::type_index;
        constexpr AutoVariant() noexcept = delete;
        template< typename AlternativeParameterType, typename... InitializersParameterTypes >
        constexpr AutoVariant( std::in_place_type_t< AlternativeParameterType >, InitializersParameterTypes... initializers ) noexcept
                : data( static_cast< void* >( new AlternativeParameterType( 
                        std::forward< InitializersParameterTypes >( initializers )... ) 
                    ) ), 
                alternative_index( type_index< AlternativeParameterType > ) {}
        constexpr ~AutoVariant() noexcept
        {
            visit< []( auto* data_ ) {
                    delete static_cast< decltype( data_ ) >( data_ );
                    return nullptr; 
                }>( *this );
        }
        constexpr AutoVariant( const AutoVariant& other ) noexcept = default;
        constexpr AutoVariant( AutoVariant&& other ) noexcept = default;
        constexpr ThisType& operator=( const ThisType& other ) = default;
        constexpr ThisType& operator=( ThisType&& other ) = default;

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
    constexpr bool holds_alternative( const auto& variant ) {
        return std::remove_reference_t< std::decay_t< decltype( variant ) > >::template type_index< AlternativeType > == variant.index();
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
                const AutoVariant< ParameterTypes... >& variant 
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
                const AutoVariant< ParameterTypes... >& variant 
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
            const AutoVariant< ParameterTypes... >& variant 
        ) noexcept
    {
        
        using FirstAlternativeType = typename IndexToType< 0, 0, ParameterTypes... >::Type;
        FirstAlternativeType* substitute = nullptr;
        using ReturnType = decltype( VisitorParameterConstant( substitute ) );
        return VisitImplementation< 
                ReturnType, 
                0, 
                sizeof...( ParameterTypes ) - 1, 
                VisitorParameterConstant, 
                ParameterTypes... 
            >( variant ).result;
    }

    /*template< auto VisitorParameterConstant, typename... ParameterTypes >
    constexpr static auto visit( 
                const AutoVariant< ParameterTypes... >* variant 
            ) noexcept {
        const AutoVariant< ParameterTypes... >& refrence = *variant;
        return visit( refrence );
    }*/

    template< typename QueryParameterType, typename... VariantAlternativeParameterTypes >
    constexpr static QueryParameterType* get_if( const AutoVariant< VariantAlternativeParameterTypes... >* variant ) {
        if( decltype( FindTypeIndexDecay< 0, QueryParameterType, VariantAlternativeParameterTypes... >{} )::type_index == variant->index() )
            return static_cast< QueryParameterType* >( variant->get_data() );
        return nullptr;
    }
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_AUTO_VARIANT_HPP
