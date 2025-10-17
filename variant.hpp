#include <algorithm>
#include <initializer_list>
#include <new>
#include <type_traits>

template <size_t Index, typename... Types>
struct TypeAt;

template <size_t Index, typename Head, typename... Tail>
struct TypeAt<Index, Head, Tail...> {
	using value = typename TypeAt<Index - 1, Tail...>::value;
};

template <typename Head, typename... Tail>
struct TypeAt<0, Head, Tail...> {
	using value = typename Head;
};

template <size_t Index>
struct TypeAt<Index> {
	static_assert(false, "Type not found");
};

template <typename Type, typename... Types>
struct IndexAt;

template <typename Type, typename Head, typename... Tail>
struct IndexAt<Type, Head, Tail...> {
	static constexpr size_t value = 1 + IndexAt<Type, Tail...>::value;
};

template <typename Type, typename... Tail>
struct IndexAt<Type, Type, Tail...> {
	static constexpr size_t value = 0;
};

template <typename Type>
struct IndexAt<Type> {
	static_assert(false, "Index not found");
};

template <typename Type, typename... Types>
struct Contains;

template <typename Type, typename Head, typename... Types>
struct Contains<Type, Head, Types...> {
	static constexpr bool value = Contains<Type, Types...>::value;
};

template <typename Type, typename... Types>
struct Contains<Type, Type, Types...> {
	static constexpr bool value = true;
};

template <typename Type>
struct Contains<Type> {
	static constexpr bool value = false;
};

template <typename... Types>
class Variant final {
private:
	static constexpr size_t _count = sizeof...(Types);
	static constexpr size_t _size = std::max({ sizeof(Types)... });
	static constexpr size_t _align = std::max({ alignof(Types)... });

	std::aligned_storage<_size, _align> _value;
	size_t _current;

public:
	template<typename Type>
	explicit Variant(const Type& value) {
		_current = IndexAt<Type, Types...>::value;
		new (reinterpret_cast<char*>(&_value)) Type(value);
	}

	Variant(const Variant& variant) : _current(variant._current) {
		size_t index = 0;
		char* _ptr = reinterpret_cast<char*>(&_value);

		((index++ == _current ?
			(new (_ptr) Types(*reinterpret_cast<const Types*>(&variant._value)), void()) :
			void()),
			...);
	}

	Variant(Variant&& variant) : _current(variant._current) {
		size_t index = 0;
		char* _ptr = reinterpret_cast<char*>(&_value);

		((index++ == _current ?
			(new (_ptr) Types(std::move(*reinterpret_cast<Types*>(&variant._value))), void()) :
			void()),
			...);

		variant._current = static_cast<size_t>(-1);
		std::memset(reinterpret_cast<char*>(&_value), 0, _size);
	}

	template <size_t Index>
	auto& get() noexcept {
		// static_assert(Index == _current, "Invalid index");
		return *reinterpret_cast<TypeAt<Index, Types...>::value*>(&_value);
	}

	template <typename Type>
	size_t get() const noexcept {
		size_t index = IndexAt<Type, Types...>::value;
		// static_assert(index == _current, "Invalid index");
		return index;
	}

	template <typename Type>
	bool holds_alternative() const noexcept {
		return Contains<Type, Types...>::value;
	}

	size_t get_index() const noexcept {
		return _current;
	}

	Variant& operator=(const Variant& variant) {
		if (this == &variant) {
			return *this;
		}

		_current = variant._current;

		size_t index = 0;
		char* _ptr = reinterpret_cast<char*>(&_value);

		((index++ == _current ?
			(new (_ptr) Types(*reinterpret_cast<const Types*>(&variant._value)), void()) :
			void()),
			...);

		return *this;
	}

	Variant& operator=(Variant&& variant) {
		if (this == &variant) {
			return *this;
		}

		_current = variant._current;

		size_t index = 0;
		char* _ptr = reinterpret_cast<char*>(&_value);

		((index++ == _current ?
			(new (_ptr) Types(std::move(*reinterpret_cast<Types*>(&variant._value))), void()) :
			void()),
			...);

		variant._current = static_cast<size_t>(-1);
		std::memset(reinterpret_cast<char*>(&_value), 0, _size);

		return *this;
	}
};
