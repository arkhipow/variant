#pragma once

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



template <typename... Args>
struct type_list {};



template <typename T, typename... Args>
struct make_type_list;

template <typename T, typename... Args>
struct make_type_list<T, type_list<Args...>> {
	using type = type_list<T, Args...>;
};



template <typename T, typename... Args>
struct remove_type;

template <typename T, typename First, typename... Rest>
struct remove_type<T, First, Rest...> {
	using type = typename make_type_list<First, typename remove_type<T, Rest...>::type>::type;
};

template <typename T, typename... Rest>
struct remove_type<T, T, Rest...> {
	using type = typename remove_type<T, Rest...>::type;
};

template <typename T>
struct remove_type<T> {
	using type = type_list<>;
};



template <typename... Args>
struct unique_types;

template <typename T, typename... Args>
struct unique_types<T, Args...> {
	using type = typename make_type_list<T, typename unique_types<typename remove_type<T, Args...>::type>::type>::type;
};

template <typename T, typename... Args>
struct unique_types<type_list<T, Args...>> {
	using type = typename unique_types<T, Args...>::type;
};

template <typename T>
struct unique_types<T> {
	using type = type_list<T>;
};

template <>
struct unique_types<type_list<>> {
	using type = type_list<>;
};

template <>
struct unique_types<> {
	using type = type_list<>;
};
