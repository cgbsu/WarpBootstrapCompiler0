#include <Warp/Common.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_NOT_SO_UNIQUE_POINTER_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_NOT_SO_UNIQUE_POINTER_HPP

namespace Warp::Utilities
{
    /* Before you ask... no, I could not use unique_ptr, in fact I could not manage ****
    ** the memory, for why you cant use unique_ptr see: ********************************
    ** See: https://github.com/cplusplus/papers/issues/961 *****************************
    ** and http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2273r0.pdf *********
    ** https://github.com/riscygeek/constexpr_suff/blob/master/include/unique_ptr.hpp */


    template< typename StorageType >
    struct NotSoUniquePointer
    {
        constexpr NotSoUniquePointer() : pointer( nullptr ) {}
        constexpr NotSoUniquePointer( StorageType* pointer ) noexcept : pointer( pointer ) {}

        template< typename... InitializersParameterTypes >
        constexpr NotSoUniquePointer( std::in_place_type_t< StorageType >, InitializersParameterTypes... initializers ) noexcept
                : pointer( new StorageType( std::forward< InitializersParameterTypes >( initializers )... ) ) {}

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
        constexpr StorageType* operator->() const noexcept {
            return pointer;
        }

        constexpr const StorageType* get_pointer() const noexcept {
            return pointer;
        }
        protected: 
            StorageType* pointer;
    };

    template<>
    struct NotSoUniquePointer< const char* >
    {
        constexpr NotSoUniquePointer() : pointer( nullptr ) {}
        constexpr NotSoUniquePointer( auto... string ) noexcept : pointer( new char[ sizeof...( string ) ]{ string... } ) {}
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
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_NOT_SO_UNIQUE_POINTER_HPP
