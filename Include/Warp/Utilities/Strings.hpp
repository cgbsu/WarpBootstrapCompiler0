#include <Warp/Utilities/NotSoUniquePointer.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_NOT_SO_STRINGS_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_NOT_SO_STRINGS_HPP

namespace Warp::Utilities
{
    constexpr std::optional< bool > to_bool( 
            std::string_view boolean_token, 
            std::string_view true_token, 
            std::string_view false_token 
        )
    {
        return ( 
            boolean_token == true_token ? 
                    std::optional{ true } : ( 
                            boolean_token == false_token ? 
                                    std::optional{ false } : std::nullopt 
                    ) 
            );
    }

    template< std::integral ParameterType >//, auto out = []( ParameterType val ) { std::cout << "VAL: " << val << "\n"; return 0; } >
    constexpr ParameterType to_integral( std::string_view integer_token )
    {
        ParameterType sum = 0;
        for( auto digit : integer_token )
            sum = ( sum * 10 ) + digit - '0';
        // out( sum );
        return sum;
    }

    template< typename ParameterType >
    constexpr HeapStringType to_string( ParameterType to_stringify ) {
        return HeapStringType{ std::string_view{ to_stringify }.data() };
    }

    template<>
    constexpr HeapStringType to_string( char to_stringify ) {
        return HeapStringType{ to_stringify, '\0' };
    }

    template< typename ParameterType >
    concept Enumaration = std::is_enum< ParameterType >::value;

    template< Enumaration EnumerationParameterType >
    constexpr HeapStringType to_string( EnumerationParameterType to_stringify )
    {
        using UnderylingType = std::underlying_type_t< decltype( to_stringify ) >;
        return to_string( static_cast< UnderylingType >( to_stringify ) );
    }

    template< typename = char >
    constexpr char to_char( char from ) {
        return from;
    }

    template< Enumaration EnumerationParameterType >
    constexpr char to_char( EnumerationParameterType from ) {
        using UnderylingType = std::underlying_type_t< decltype( from ) >;
        return to_char( static_cast< UnderylingType >( from ) );
    }
    template< 
            std::integral IntegralParameterType, 
            std::integral auto BaseParameterConstant = 10, 
            char BaseStringParameterConstant = '0' 
        >
    constexpr HeapStringType integral_to_string_implementation( 
            const IntegralParameterType to_stringify, 
            const std::integral auto number_of_digits, 
            const auto... string_digits 
        ) noexcept 
    {
        if( to_stringify < BaseParameterConstant )
            return HeapStringType{ string_digits..., to_stringify + BaseStringParameterConstant, '\0' };

        const auto raised = raise( BaseParameterConstant, number_of_digits );
        const auto high_number = BaseParameterConstant * static_cast< const IntegralParameterType >( 
                to_stringify / ( BaseParameterConstant * raised ) 
            );
        const auto digit = static_cast< const IntegralParameterType >( to_stringify / raised ) - high_number;

        return integral_to_string_implementation( 
                to_stringify - ( high_number * raised ), 
                number_of_digits - 1, 
                string_digits..., digit + BaseStringParameterConstant 
            );
    }

    template< 
            std::integral IntegralParameterType, 
            std::integral auto BaseParameterConstant = 10, 
            char BaseStringParameterConstant = '0' 
        >
    constexpr HeapStringType integral_to_string( IntegralParameterType to_stringify )
    {
        const auto number_of_digits = log< IntegralParameterType, BaseParameterConstant >( to_stringify );
        return integral_to_string_implementation< 
                IntegralParameterType, 
                BaseParameterConstant, 
                BaseStringParameterConstant 
            >( to_stringify, number_of_digits );
    }


    template< std::integral IntegralParameterType >
    constexpr HeapStringType to_string( IntegralParameterType to_stringify ) {
        return integral_to_string( to_stringify );
    }

    template< typename... AlternativeParameterTypes >
    constexpr HeapStringType to_string( const AutoVariant< AlternativeParameterTypes... >* to_stringify ) {
        const AutoVariant< AlternativeParameterTypes... >& refrence = *to_stringify;
        return to_string( to_stringify );
    }

    template< typename... AlternativeParameterTypes >
    constexpr HeapStringType to_string( const AutoVariant< AlternativeParameterTypes... >& to_stringify ) {
        return visit( []( auto data ) { return to_string< decltype( data ) >( data ); }, to_stringify );
    }


    template< typename PointerParameterType >
    constexpr HeapStringType to_string( NotSoUniquePointer< PointerParameterType > to_stringify ) {
        return to_string( to_stringify.get_pointer() );
    }
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_NOT_SO_STRINGS_HPP
