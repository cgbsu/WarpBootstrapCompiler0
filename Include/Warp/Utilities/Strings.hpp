#include <Warp/Utilities/NotSoUniquePointer.hpp>
#include <Warp/Utilities/Mathematical.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_NOT_SO_STRINGS_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_NOT_SO_STRINGS_HPP

namespace Warp::Utilities
{
    template<>
    struct NotSoUniquePointer< const char* >
    {
        constexpr NotSoUniquePointer() : pointer( nullptr ) {}
        constexpr NotSoUniquePointer( auto... string ) noexcept : pointer( new char[ sizeof...( string ) ]{ static_cast< char >( string )... } ) {}
        constexpr NotSoUniquePointer( const char* string ) noexcept : pointer( string ) {}
        constexpr NotSoUniquePointer( const NotSoUniquePointer& other ) noexcept : pointer( other.pointer ) {
            ( ( NotSoUniquePointer& ) other ).pointer = nullptr;
        }
        constexpr NotSoUniquePointer( NotSoUniquePointer&& other ) noexcept : pointer( other.pointer ) {
            other.pointer = nullptr;
        }
        constexpr ~NotSoUniquePointer() noexcept {
            delete pointer; 
        }
        constexpr NotSoUniquePointer& operator=( const NotSoUniquePointer& other ) noexcept
        {
            pointer = other.pointer;
            ( ( NotSoUniquePointer& ) other ).pointer = nullptr;
            return *this;
        }
        constexpr NotSoUniquePointer& operator=( NotSoUniquePointer&& other ) noexcept
        {
            pointer = other.pointer;
            other.pointer = nullptr;
            return *this;
        }
        constexpr const char* operator->() const noexcept {
            return pointer;
        }

        constexpr const char* get_pointer() const noexcept {
            return pointer;
        }

        constexpr const std::string_view to_string_view() const noexcept {
            return std::string_view{ pointer };
        }

        constexpr operator const std::string_view() const noexcept {
            return to_string_view();
        }

        protected: 
            const char* pointer;
    };

    using HeapStringType = NotSoUniquePointer< const char* >;


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
            char BaseStringParameterConstant = '0', 
            size_t RecursionMaxParameterConstant = 850 
        >
    constexpr HeapStringType integral_to_string_implementation( 
            const IntegralParameterType to_stringify, 
            const std::integral auto number_of_digits, 
            const auto... string_digits 
        ) noexcept 
    {
        if constexpr( RecursionMaxParameterConstant > 0 )
        {
            if( to_stringify < BaseParameterConstant )
                return HeapStringType{ string_digits..., to_stringify + BaseStringParameterConstant, '\0' };

            const size_t raised = raise( BaseParameterConstant, number_of_digits );
            const auto high_number = BaseParameterConstant * static_cast< const IntegralParameterType >( 
                    to_stringify / ( BaseParameterConstant * raised ) 
                );
            const char digit = static_cast< const IntegralParameterType >( to_stringify / raised ) - high_number;

            return integral_to_string_implementation< 
                    IntegralParameterType, 
                    BaseParameterConstant, 
                    BaseStringParameterConstant, 
                    RecursionMaxParameterConstant -1 
                >( 
                    to_stringify - ( high_number * raised ), 
                    number_of_digits - 1, 
                    string_digits..., digit + BaseStringParameterConstant 
                );

        }
        else 
            return HeapStringType{ string_digits..., to_stringify + BaseStringParameterConstant, '\0' };
    }

    template< 
            std::integral IntegralParameterType, 
            std::integral auto BaseParameterConstant = 10, 
            char BaseStringParameterConstant = '0', 
            size_t RecursionMaxParameterConstant = 90  
        >
    constexpr HeapStringType integral_to_string( IntegralParameterType to_stringify )
    {
        const auto number_of_digits = log< IntegralParameterType, BaseParameterConstant, 0, RecursionMaxParameterConstant >( to_stringify );
        return integral_to_string_implementation< 
                IntegralParameterType, 
                BaseParameterConstant, 
                BaseStringParameterConstant, 
                RecursionMaxParameterConstant 
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


    // https://cp-algorithms.com/string/string-hashing.html //
    // https://stackoverflow.com/questions/1835976/what-is-a-sensible-prime-for-hashcode-calculation/2816747# //

    using HashedStringType = unsigned long long int;

    template< 
            HashedStringType BaseParameterConstant = 92821, 
            HashedStringType TableSizeOrderParameterConstant = 486187739, 
            size_t OffsetParameterConstant = 'a' + 1 
        >
    constexpr HashedStringType hash_string( std::string_view to_hash )
    {
        HashedStringType power = BaseParameterConstant;
        HashedStringType hash = 0;
        for( size_t ii = 0; ii < to_hash.size(); ++ii )
        {
            hash += ( ( ( to_hash[ ii ] - OffsetParameterConstant ) * power ) 
                    % TableSizeOrderParameterConstant
                );
            power *= BaseParameterConstant;
        }
        return hash;
    }
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_NOT_SO_STRINGS_HPP
