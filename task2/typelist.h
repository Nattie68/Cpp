#pragma once

#include <type_traits>
#include <cstddef>

namespace typelist {

template<typename... Types>
struct TypeList {};

namespace detail {
    template<typename T, typename List>
    struct IndexOfHelper;

    template<typename T>
    struct IndexOfHelper<T, TypeList<>> {
        static constexpr std::size_t value = static_cast<std::size_t>(-1);
    };

    template<typename T, typename First, typename... Rest>
    struct IndexOfHelper<T, TypeList<First, Rest...>> {
        static constexpr std::size_t value = 
            std::is_same_v<T, First> 
            ? 0 
            : (IndexOfHelper<T, TypeList<Rest...>>::value == static_cast<std::size_t>(-1)
                ? static_cast<std::size_t>(-1)
                : 1 + IndexOfHelper<T, TypeList<Rest...>>::value);
    };
}

template<typename List, std::size_t Index>
struct TypeAt;

template<typename First, typename... Rest, std::size_t Index>
struct TypeAt<TypeList<First, Rest...>, Index> {
    static_assert(Index < sizeof...(Rest) + 1, "Index out of range");
    using type = typename TypeAt<TypeList<Rest...>, Index - 1>::type;
};

template<typename First, typename... Rest>
struct TypeAt<TypeList<First, Rest...>, 0> {
    using type = First;
};

template<std::size_t Index>
struct TypeAt<TypeList<>, Index> {
    static_assert(sizeof(Index) == 0, "Index out of range");
};

template<typename List>
struct Size;

template<typename... Types>
struct Size<TypeList<Types...>> {
    static constexpr std::size_t value = sizeof...(Types);
};

template<typename T, typename List>
struct Contains;

template<typename T, typename... Types>
struct Contains<T, TypeList<Types...>> {
    static constexpr bool value = (std::is_same_v<T, Types> || ...);
};

template<typename T, typename List>
struct IndexOf {
    static constexpr std::size_t value = detail::IndexOfHelper<T, List>::value;
    static_assert(value != static_cast<std::size_t>(-1), "Type not found in list");
};

template<typename List, typename NewType>
struct Append;

template<typename... Types, typename NewType>
struct Append<TypeList<Types...>, NewType> {
    using type = TypeList<Types..., NewType>;
};

template<typename List, typename NewType>
struct Prepend;

template<typename... Types, typename NewType>
struct Prepend<TypeList<Types...>, NewType> {
    using type = TypeList<NewType, Types...>;
};

} 
