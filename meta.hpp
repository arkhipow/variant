#pragma once
#include <type_traits>

template <typename T, typename... Args>
struct index_of;

template <typename T, typename First, typename... Rest>
struct index_of<T, First, Rest...> {
	static constexpr size_t value = index_of<T, Rest...>::value + 1;
};

template <typename T, typename... Rest>
struct index_of<T, T, Rest...> {
	static constexpr size_t value = 0;
};

template <typename T>
struct index_of<T> {
	static_assert(false, "Index not found");
};



template <size_t Index, typename... Args>
struct type_of;

template <size_t Index, typename First, typename... Rest>
struct type_of<Index, First, Rest...> {
	using type = typename type_of<Index - 1, Rest...>::type;
};

template <typename First, typename... Rest>
struct type_of<0, First, Rest...> {
	using type = First;
};

template <size_t Index>
struct type_of<Index> {
	static_assert(false, "Type not found");
};
