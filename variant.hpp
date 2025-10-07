#pragma once
#include <algorithm>
#include <stdexcept>

template <typename... Types>
class Variant final {
private:
	static constexpr size_t _size = std::max({ sizeof(Types)... });
	size_t _index;

	char _buffer[_size];

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
	void destroy(auto* buffer) {
		reinterpret_cast<T*>(buffer)->~T();
	}

public:
	template <typename T>
	Variant(const T& value) : _index(getTypeIndex<T, Types...>()) {
		new (_buffer) T(value);
	}

	Variant(const Variant& obj) : _index(obj._index) {
		size_t index = 0;
		((index++ == _index ? new (_buffer) Types(*reinterpret_cast<Types*>(obj._buffer)) : void()), ...);
	}

	Variant(Variant&& obj) : _index(obj._index) {
		size_t index = 0;
		((index++ == _index ? new (_buffer) Types(std::move(*reinterpret_cast<Types*>(obj._buffer))) : void()), ...);

		obj._index = static_cast<size_t>(-1);
	}

	~Variant() {
		size_t index = 0;
		((index++ == _index ? destroy<Types>(_buffer) : void()), ...);
	}

	Variant& operator=(const Variant& obj) {
		if (this == &obj) { return *this; }

		this->~Variant();
		_index = obj._index;

		size_t index = 0;
		((index++ == _index ? new (_buffer) Types(*reinterpret_cast<Types*>(obj._buffer)) : void()), ...);

		return *this;
	}

	Variant& operator=(Variant&& obj) {
		if (this == &obj) { return *this; }

		_index = obj._index;
		this->~Variant();

		size_t index = 0;
		((index++ == _index ? (new (_buffer) Types(std::move(*reinterpret_cast<Types*>(obj._buffer))), void()) : void()), ...);

		obj._index = static_cast<size_t>(-1);

		return *this;
	}

	template<typename T>
	T& get() {
		if (getTypeIndex<T, Types...>() != _index) {
			throw (std::runtime_error("Type not active in variant"));
		}

		return *reinterpret_cast<T*>(_buffer);
	}

	template <typename T>
	bool holdsAlternative() {
		return (std::is_same_v<T, Types> || ...);
	}

	size_t getIndex() {
		return _index;
	}
};
