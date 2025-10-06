#include <iostream>
#include <type_traits>
#include <vector>
#include <list>
#include <tuple>
#include <string>
#include <cstdint>

template<typename T, typename = void>
struct is_container : std::false_type {};

template<typename T>
struct is_container<T, std::void_t<
            typename T::value_type,
            typename T::iterator,
            decltype(std::declval<T>().begin()),
            decltype(std::declval<T>().end())>> : std::true_type {};


template<typename T>
struct is_string : std::false_type {};

template<>
struct is_string<std::string> : std::true_type {};

template<typename T, typename = void>
struct is_tuple : std::false_type {};

template<typename T>
struct is_tuple<T, std::void_t<decltype(std::tuple_size<T>::value)> > : std::true_type {};

template<typename... Ts>
struct all_same : std::false_type {};

template<typename T>
struct all_same<T> : std::true_type {};

template<typename T, typename U, typename... Rest>
struct all_same<T, U, Rest...> :
        std::conditional_t<std::is_same_v<T, U>, all_same<U, Rest...>, std::false_type> {
};

template<typename T>
std::enable_if_t<std::is_integral_v<T> >
print_ip(T value) {
    using U = std::make_unsigned_t<T>;
    U uvalue = static_cast<U>(value);

    for (int i = sizeof(T) - 1; i >= 0; --i) {
        std::cout << ((uvalue >> (8 * i)) & 0xFF);
        if (i > 0)
            std::cout << '.';
    }
    std::cout << std::endl;
}

template<typename T>
std::enable_if_t<is_string<T>::value>
print_ip(const T &value) {
    std::cout << value << std::endl;
}

template<typename T>
std::enable_if_t<is_container<T>::value && !is_string<T>::value>
print_ip(const T &container) {

    if (container.empty()) {
        std::cout << std::endl;
        return;
    }

    auto it = container.begin();
    std::cout << *it;
    ++it;

    for (; it != container.end(); ++it)
        std::cout << '.' << *it;

    std::cout << std::endl;
}

template<typename Tuple, std::size_t... I>
void print_tuple_impl(const Tuple& t, std::index_sequence<I...>)
{
    ((std::cout << (I == 0 ? "" : ".") << std::get<I>(t)), ...);
}

template<typename... Ts>
std::enable_if_t<all_same<Ts...>::value>
print_ip(const std::tuple<Ts...>& t)
{
    print_tuple_impl(t, std::index_sequence_for<Ts...>{});
    std::cout << std::endl;
}

int main()
{
    print_ip(int8_t{-1});                        // 255
    print_ip(int16_t{0});                        // 0.0
    print_ip(int32_t{2130706433});               // 127.0.0.1
    print_ip(int64_t{8875824491850138409});      // 123.45.67.89.101.112.131.41
    //
    print_ip(std::string{"Hello, World!"});      // Hello, World!
    print_ip(std::vector<int>{100, 200, 300, 400});  // 100.200.300.400
    print_ip(std::list<short>{400, 300, 200, 100});  // 400.300.200.100
    print_ip(std::make_tuple(123, 456, 789, 0));     // 123.456.789.0

    return 0;
}

