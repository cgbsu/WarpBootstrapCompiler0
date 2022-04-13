/* Big thanks to riscygeek (Benjamin Stürz) on github *****************************
** https://github.com/riscygeek/constexpr_suff/blob/master/include/unique_ptr.hpp *
** https://github.com/riscygeek **************************************************/

#ifndef FILE_UNIQUE_PTR_HPP
#define FILE_UNIQUE_PTR_HPP
#include <stdexcept>
#include <concepts>
#include <cstddef>
#include <utility>

namespace benni {
	template<class T, class U>
	concept Deleter = requires(T a, U* ptr) {
		T{};
		a(ptr);
	};
	template<class A, class B>
	concept Comparable = requires(A a, B b) {
		a <=> b;
	};

	template<class T>
	struct default_delete {
		constexpr default_delete() noexcept = default;
		constexpr default_delete(const default_delete&) noexcept = default;
		template<class U>
		constexpr default_delete(const default_delete<U>&) noexcept {}
		constexpr void operator()(T* ptr) noexcept { delete ptr; }
	};

	template<class T, Deleter<T> D = default_delete<T>>
	class unique_ptr {
	private:
		T* ptr;
		D deleter{};
	public:
		using pointer = T*;
		using element_type = T;
		using deleter_type = D;
		constexpr unique_ptr() noexcept : ptr() {}
		constexpr unique_ptr(std::nullptr_t) noexcept : ptr() {}
		constexpr unique_ptr(unique_ptr&& ptr) noexcept : ptr(ptr.ptr) { ptr.ptr = nullptr; }
		constexpr explicit unique_ptr(T* ptr) noexcept : ptr(ptr) {}
		constexpr unique_ptr(T* ptr, const D& deleter) noexcept : ptr(ptr), deleter(deleter) {}
		constexpr unique_ptr(T* ptr, D&& deleter) noexcept : ptr(ptr), deleter(std::move(deleter)) {}
		template<class U, class E>
		constexpr unique_ptr(unique_ptr<U, E>&& ptr) noexcept
			: ptr(ptr.release()), deleter(ptr.get_deleter()) {
			static_assert(std::is_base_of_v<T, E>);
		}
		constexpr ~unique_ptr() noexcept { deleter(ptr); }

		constexpr unique_ptr& operator=(std::nullptr_t) noexcept {
			deleter(ptr);
			ptr = nullptr;
			return *this;
		}
		constexpr unique_ptr& operator=(unique_ptr&& p) noexcept {
			deleter(ptr);
			ptr = p.ptr;
			p.ptr = nullptr;
			return *this;
		}
		template<class E, class U>
		constexpr unique_ptr& operator=(unique_ptr<E, U>&& p) noexcept {
			static_assert(std::is_base_of_v<T, E>);
			deleter(ptr);
			ptr = p.release();
			deleter = p.get_deleter();
			return *this;
		}
		constexpr pointer release() noexcept {
			return std::exchange(ptr, nullptr);
		}
		[[nodiscard]]
		constexpr const deleter_type& get_deleter() const noexcept { return deleter; }
		[[nodiscard]]
		constexpr deleter_type& get_deleter() noexcept { return deleter; }
		[[nodiscard]]
		constexpr pointer get() const noexcept { return ptr; }
		[[nodiscard]]
		constexpr explicit operator bool() const noexcept { return ptr != nullptr; }
		constexpr void swap(unique_ptr& other) noexcept {
			std::swap(ptr, other.ptr);
			std::swap(deleter, other.deleter);
		}
		[[nodiscard]]
		constexpr T& operator*() const {
			return ptr ? *ptr : throw std::runtime_error("dereferencing nullptr");
		}
		[[nodiscard]]
		constexpr T* operator->() const noexcept { return ptr; }
	};
	template<class T, class... Args>
	constexpr unique_ptr<T> make_unique(Args&&... args) {
		return unique_ptr<T>{ new T{ args... } };
	}

	template<class T1, class D1, class T2, class D2>
		requires Comparable<typename unique_ptr<T1, D1>::pointer, typename unique_ptr<T2, D2>::pointer>
	constexpr auto operator<=>(const unique_ptr<T1, D1>& a, const unique_ptr<T2, D2>& b) noexcept {
		return a.get() <=> b.get();
	}
	template<class T1, class D1, class T2, class D2>
	constexpr bool operator==(const unique_ptr<T1, D1>& a, const unique_ptr<T2, D2>& b) noexcept {
		return a.get() == b.get();
	}

	template<class T, class D>
		requires Comparable<typename unique_ptr<T, D>::pointer, std::nullptr_t>
	constexpr auto operator<=>(const unique_ptr<T, D>& a, std::nullptr_t b) noexcept {
		return a.get() <=> b;
	}
	template<class T, class D>
	constexpr bool operator==(const unique_ptr<T, D>& a, std::nullptr_t b) noexcept {
		return a.get() == b;
	}

	template<class T, class D>
		requires Comparable<std::nullptr_t, typename unique_ptr<T, D>::pointer>
	constexpr auto operator<=>(std::nullptr_t a, const unique_ptr<T, D>& b) noexcept {
		return a <=> b.get();
	}
	template<class T, class D>
	constexpr bool operator==(std::nullptr_t a, const unique_ptr<T, D>& b) noexcept {
		return a == b.get();
	}
}

template<class T, class D>
constexpr void std::swap(benni::unique_ptr<T, D>& a, benni::unique_ptr<T, D>& b) noexcept {
	a.swap(b);
}

#endif /* FILE_UNIQUE_PTR_HPP */
