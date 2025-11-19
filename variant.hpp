#pragma once
#include "meta.hpp"
#include <stdexcept>
#include <utility>

template <typename... Args>
class Variant {
private:
    static constexpr size_t _max_size = std::max({ sizeof(Args)... });
    static constexpr size_t _max_count = std::max({ sizeof...(Args) });
    static constexpr size_t _max_align = std::max({ alignof(Args)... });

    size_t _curr_type_index;
    alignas(_max_align) char _storage[_max_size];

    template <size_t Index>
    constexpr void copy(const Variant& rhs) {
        if constexpr (Index < _max_count) {
            if (Index == rhs._curr_type_index) {
                using T = typename type_of<Index, Args...>::type;
                new (_storage) T(*reinterpret_cast<const T*>(rhs._storage));
            }

            else {
                copy<Index + 1>(rhs);
            }
        }
    }

    template <size_t Index>
    constexpr void move(Variant&& rhs) {
        if constexpr (Index < _max_count) {
            if (Index == rhs._curr_type_index) {
                using T = typename type_of<Index, Args...>::type;
                new (_storage) T(std::move(*reinterpret_cast<T*>(rhs._storage)));
            }

            else {
                move<Index + 1>(std::move(rhs));
            }
        }
    }

    template <typename T>
    constexpr void destroy() noexcept {
        reinterpret_cast<T*>(_storage)->~T();
    }

    template <size_t Index>
    constexpr void destroy_by_index() {
        if constexpr (Index < _max_count) {
            if (Index == _curr_type_index) {
                using T = typename type_of<Index, Args...>::type;
                destroy<T>();
            }

            else {
                destroy_by_index<Index + 1>();
            }
        }
    }

public:
    template <typename T>
    Variant(const T& value) noexcept {
        _curr_type_index = index_of<T, Args...>::value;
        new (_storage) T(value);
    }

    Variant(const Variant& variant) noexcept {
        _curr_type_index = variant._curr_type_index;
        copy<0>(variant);
    }

    Variant(Variant&& variant) noexcept {
        _curr_type_index = variant._curr_type_index;
        move<0>(std::move(variant));
        
        variant._curr_type_index = static_cast<size_t>(-1);
    }

    ~Variant() noexcept {
        destroy_by_index<0>();
    }

    template <typename T>
    constexpr auto& get() {
        if (index_of<T, Args...>::value != _curr_type_index) {
            throw std::runtime_error("Type is not current");
        }

        return *reinterpret_cast<T*>(_storage);
    }

    template <size_t Index>
    constexpr auto& get() {
        if (Index != _curr_type_index) {
            throw std::runtime_error("Index is not current");
        }

        using type = typename type_of<Index, Args...>::type;

        return *reinterpret_cast<type*>(_storage);
    }

    template <typename T>
    constexpr bool holds_alternative() const noexcept {
        return index_of<T, Args...>::value == _curr_type_index;
    }

    constexpr size_t index() const noexcept {
        return _curr_type_index;
    }

    Variant& operator=(const Variant& rhs) noexcept {
        if (this != &rhs) {
            destroy_by_index<0>();

            _curr_type_index = rhs._curr_type_index;
            copy<0>(rhs);
        }

        return *this;
    }

    Variant& operator=(Variant&& rhs) noexcept {
        if (this != &rhs) {
            destroy_by_index<0>();

            _curr_type_index = rhs._curr_type_index;
            move<0>(std::move(rhs));

            rhs._curr_type_index = static_cast<size_t>(-1);
        }

        return *this;
    }
};
