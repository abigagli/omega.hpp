/* omega.hpp */
// a tiny tuple meta library
// written by Gian Lorenzo Meocci <glmeocci@gmail.com>

#include <tuple>
#include <utility>
#include <type_traits>


namespace omega
{
    template <int i>
    using int_c = std::integral_constant<int, i>;

    /* make_list */

    template <int B, int E, typename Tup>
    decltype(auto) _make_list(Tup&& tup, std::true_type)
    {
        return std::make_tuple(std::get<B>(std::forward<Tup>(tup)));
    }

    template <int B, int E, typename Tup>
    decltype(auto) _make_list(Tup&& tup, std::false_type)
    {
        return std::tuple_cat(
            std::make_tuple(std::get<B>(std::forward<Tup>(tup))),
            _make_list<B+1, E>(std::forward<Tup>(tup), std::is_same<int_c<B+1>, int_c<E>>{})
        );
    }

    template <int B, int E, typename Tup>
    decltype(auto) make_list(Tup&& tup)
    {
        static_assert(B < E, "");
        return _make_list<B, E>(std::forward<Tup>(tup), std::is_same<int_c<B>, int_c<E>>{});
    }


    /* make_reverse_list */

    template <int B, int E, typename Tup>
    decltype(auto) _make_reverse_list(Tup&& tup, std::true_type)
    {
        return std::make_tuple(std::get<B>(std::forward<Tup>(tup)));
    }

    template <int B, int E, typename Tup>
    decltype(auto) _make_reverse_list(Tup&& tup, std::false_type)
    {
        return std::tuple_cat(
            std::make_tuple(std::get<B>(std::forward<Tup>(tup))),
            _make_reverse_list<B-1, E>(std::forward<Tup>(tup), std::is_same<int_c<B-1>, int_c<E>>{})
        );
    }

    template <int B, int E, typename Tup>
    decltype(auto) make_reverse_list(Tup&& tup)
    {
        static_assert(B > E, "");
        return _make_reverse_list<B, E>(std::forward<Tup>(tup), std::is_same<int_c<B>, int_c<E>>{});
    }


    /* make_index_list */

    template <class Tup>
    decltype(auto) _make_index_list(Tup&& tup)
    {
        return std::make_tuple();
    }

    template <class Tup, int B, int ...Index>
    decltype(auto) _make_index_list(Tup&& tup)
    {
        return std::tuple_cat(
            std::make_tuple(std::get<B>(std::forward<Tup>(tup))),
            _make_index_list<Tup, Index...>(std::forward<Tup>(tup))
        );
    }

    template <int B, int ...Index, class Tup>
    decltype(auto) make_index_list(Tup&& tup)
    {
        return _make_index_list<Tup, B, Index...>(std::forward<Tup>(tup));
    }


    /* tail, head, take */
    template <class Tup>
    decltype(auto) tail(Tup&& tup)
    {
        constexpr auto S = std::tuple_size<typename std::decay<Tup>::type>::value;
        return omega::make_list<1, S-1>(tup);
    }

    template <class Tup>
    decltype(auto) head(Tup&& tup)
    {
        return omega::make_index_list<0>(tup);
    }

    /* invoke function */

    template<typename Func, typename Tup, std::size_t... index>
    decltype(auto) invoke_helper(Func&& func, Tup&& tup, std::index_sequence<index...>)
    {
        return func(std::get<index>(std::forward<Tup>(tup))...);
    }


    template<typename Func, typename Tup>
    decltype(auto) invoke(Func&& func, Tup&& tup)
    {
        constexpr auto S = std::tuple_size<typename std::decay<Tup>::type>::value;
        return invoke_helper(std::forward<Func>(func),
                             std::forward<Tup>(tup),
                             std::make_index_sequence<S>{});
    }


    /* utility */

    template<typename Array, std::size_t... I>
    decltype(auto) a2t_impl(const Array& a, std::index_sequence<I...>)
    {
        return std::make_tuple(a[I]...);
    }

    template<typename T, std::size_t N, typename Indices = std::make_index_sequence<N>>
    decltype(auto) array2tuple(const std::array<T, N>& a)
    {
        return a2t_impl(a, Indices());
    }
}