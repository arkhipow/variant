#pragma once
#include "meta.hpp"
#include <utility>

template <typename... Args>
class Variant {
private:
	static constexpr size_t _max_size = std::max({ sizeof(Args)... });
	static constexpr size_t _max_align = std::max({ alignof(Args)... });

	size_t _curr_type_index;
	std::aligned_storage<_max_size, _max_align> _storage;

	template <typename T>
	void destroy() noexcept {
		reinterpret_cast<T*>(&_storage)->~T();
	}

public:
	template <typename T>
	Variant(const T& value) noexcept {
		_curr_type_index = index_of<T, Args...>::value;

		char* ptr_storage = reinterpret_cast<char*>(&_storage);
		new (ptr_storage) T(value);
	}

	Variant(const Variant& variant) noexcept {
		_curr_type_index = variant._curr_type_index;

		size_t index = 0;
		char* ptr_storage = reinterpret_cast<char*>(&_storage);
		((index++ == variant._curr_type_index ? (new (ptr_storage) Args(*reinterpret_cast<const Args*>(&variant._storage)), void()) : void()), ...);
	}

	Variant(Variant&& variant) noexcept {
		_curr_type_index = variant._curr_type_index;

		size_t index = 0;
		char* ptr_storage = reinterpret_cast<char*>(&_storage);
		((index++ == variant._curr_type_index ? (new (ptr_storage) Args(std::move(*reinterpret_cast<Args*>(&variant._storage))), void()) : void()), ...);

		variant._curr_type_index = static_cast<size_t>(-1);
	}

	~Variant() noexcept {
		size_t index = 0;
		((index++ == _curr_type_index ? destroy<Args>() : void()), ...);
	}

	template <typename T>
	constexpr auto& get() {
		if (index_of<T, Args...>::value != _curr_type_index) {
			throw ("Type is not current");
		}

		return *reinterpret_cast<T*>(&_storage);
	}

	template <size_t Index>
	constexpr auto& get() {
		if (Index != _curr_type_index) {
			throw ("Index is not current");
		}

		using type = typename type_of<Index, Args...>::type;

		return *reinterpret_cast<type*>(&_storage);
	}

	template <typename T>
	constexpr bool holds_alternative() const noexcept {
		return contains<T, Args...>::value;
	}

	constexpr size_t index() const noexcept {
		return _curr_type_index;
	}

	Variant& operator=(const Variant& variant) noexcept {
		if (this == &variant) {
			return *this;
		}

		size_t index = 0;
		((index++ == _curr_type_index ? destroy<Args>() : void()), ...);

		_curr_type_index = variant._curr_type_index;

		index = 0;
		char* ptr_storage = reinterpret_cast<char*>(&_storage);
		((index++ == variant._curr_type_index ? (new (ptr_storage) Args(*reinterpret_cast<const Args*>(&variant._storage)), void()) : void()), ...);

		return *this;
	}

	Variant& operator=(Variant&& variant) noexcept {
		if (this == &variant) {
			return *this;
		}

		size_t index = 0;
		((index++ == _curr_type_index ? destroy<Args>() : void()), ...);

		_curr_type_index = variant._curr_type_index;

		index = 0;
		char* ptr_storage = reinterpret_cast<char*>(&_storage);
		((index++ == variant._curr_type_index ? (new (ptr_storage) Args(std::move(*reinterpret_cast<Args*>(&variant._storage))), void()) : void()), ...);

		variant._curr_type_index = static_cast<size_t>(-1);

		return *this;
	}
};
