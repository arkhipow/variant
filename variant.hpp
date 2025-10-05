#pragma once
#include <algorithm>
#include <stdexcept>

template <typename T, typename... Types>
size_t getTypeIndex() {
	size_t index = 0;

	for (bool found : { std::is_same_v<T, Types>... }) {
		if (found) { return index; }
		++index;
	}

	throw (std::runtime_error("Type not found in type list"));
}

template <typename T>
void destroy(bool condition, auto* buffer) {
	if (condition) {
		reinterpret_cast<T*>(buffer)->~T();
	}
}

template <typename... Types>
class Variant final {
private:
	static constexpr size_t _count = sizeof...(Types);
	static constexpr size_t _size = std::max({ sizeof(Types)... });
	size_t _index;

	char _buffer[_size];

public:
	template <typename T>
	Variant(const T& value) : _index(getTypeIndex<T, Types...>()) {
		new (_buffer) T(value);
	}

	~Variant() {
		size_t index = 0;
		(destroy<Types>(index++ == _index, _buffer), ...);
	}

	template<typename T>
	T& get() {
		if (getTypeIndex<T, Types...>() != _index) {
			throw (std::runtime_error("Type not active in variant"));
		}

		return *reinterpret_cast<T*>(_buffer);
	}
};
