#pragma once

// dbg(...) pretty-prints its arguments to stderr, tagged with the source text.
// Active only under -DLOCAL; otherwise it expands to 42, so the same file still
// builds on judges that don't ship this header.
//
// stringify() dispatches at compile time (C++20 concepts + if constexpr) and
// handles: arithmetic, char, string/string_view, bool, any range/container
// (nested, maps, set, span, ...), pair, tuple/array of any arity, optional,
// variant, expected (C++23), bitset, and anything with an operator<<.

#include <bits/stdc++.h>
#include <unistd.h>  // isatty (not part of bits/stdc++.h)

namespace dbg_impl {
using std::string;

template <class T>
string stringify(const T& x);

// traits driving the if constexpr ladder below
template <class> inline constexpr bool is_pair_v = false;
template <class A, class B> inline constexpr bool is_pair_v<std::pair<A, B>> = true;
template <class> inline constexpr bool is_tuple_v = false;
template <class... Ts> inline constexpr bool is_tuple_v<std::tuple<Ts...>> = true;
template <class> inline constexpr bool is_optional_v = false;
template <class T> inline constexpr bool is_optional_v<std::optional<T>> = true;
template <class> inline constexpr bool is_variant_v = false;
template <class... Ts> inline constexpr bool is_variant_v<std::variant<Ts...>> = true;
template <class> inline constexpr bool is_bitset_v = false;
template <size_t N> inline constexpr bool is_bitset_v<std::bitset<N>> = true;
template <class> inline constexpr bool is_vector_bool_v = false;
template <class A> inline constexpr bool is_vector_bool_v<std::vector<bool, A>> = true;
#ifdef __cpp_lib_expected
template <class> inline constexpr bool is_expected_v = false;
template <class T, class E> inline constexpr bool is_expected_v<std::expected<T, E>> = true;
#endif

template <class T>
concept Streamable = requires(std::ostream& os, const T& x) { os << x; };

template <class T>
string stringify(const T& x) {
  if constexpr (std::is_same_v<T, bool>) {
    return x ? "true" : "false";
  } else if constexpr (std::is_same_v<T, char>) {
    return "'" + string(1, x) + "'";
  } else if constexpr (std::is_same_v<T, std::monostate>) {
    return "monostate";
  } else if constexpr (std::is_arithmetic_v<T>) {
    return std::to_string(x);
  } else if constexpr (std::is_convertible_v<T, std::string_view>) {
    return '"' + string(std::string_view(x)) + '"';
  } else if constexpr (is_optional_v<T>) {
    return x ? stringify(*x) : "nullopt";
  } else if constexpr (is_variant_v<T>) {
    return std::visit([](const auto& v) { return stringify(v); }, x);
#ifdef __cpp_lib_expected
  } else if constexpr (is_expected_v<T>) {
    return x ? stringify(*x) : "unexpected(" + stringify(x.error()) + ")";
#endif
  } else if constexpr (is_pair_v<T>) {
    return "(" + stringify(x.first) + ", " + stringify(x.second) + ")";
  } else if constexpr (is_tuple_v<T>) {
    return std::apply(
        [](const auto&... xs) {
          string res = "(";
          bool sep = false;
          ((res += (sep ? ", " : "") + stringify(xs), sep = true), ...);
          return res + ")";
        },
        x);
  } else if constexpr (is_vector_bool_v<T>) {
    string res = "{";
    bool sep = false;
    for (bool b : x) {
      if (sep) res += ", ";
      sep = true;
      res += stringify(b);
    }
    return res + "}";
  } else if constexpr (is_bitset_v<T>) {
    // keep the original least-significant-bit-first order (x[0] leftmost)
    string res(x.size(), '0');
    for (size_t i = 0; i < x.size(); i++) res[i] = char('0' + x[i]);
    return res;
  } else if constexpr (std::ranges::range<T>) {
    string res = "{";
    bool sep = false;
    for (const auto& e : x) {
      if (sep) res += ", ";
      sep = true;
      res += stringify(e);
    }
    return res + "}";
  } else if constexpr (Streamable<T>) {
    std::ostringstream os;
    os << x;
    return os.str();
  } else {
    static_assert(!sizeof(T*), "dbg: no way to stringify this type");
  }
}

// color debug lines red when stderr is a terminal (matches nvim's old :!
// behavior of painting stderr with ErrorMsg); plain when piped/redirected
// so files never get escape-code garbage
inline const char* color(bool open) {
  static const bool tty = isatty(fileno(stderr));
  return tty ? (open ? "\033[31m" : "\033[0m") : "";
}

template <class... Ts>
void out(const Ts&... xs) {
  ((std::cerr << ' ' << stringify(xs)), ...);
  std::cerr << color(false) << std::endl;
}
}  // namespace dbg_impl

#ifdef LOCAL
// flush cout first: with sync_with_stdio(0) cout is buffered until exit while
// cerr is unbuffered, so without this every debug line prints before any real
// output instead of interleaving in program order
#define dbg(...)                                                          \
  (std::cout.flush(),                                                     \
   std::cerr << dbg_impl::color(true) << "[" << #__VA_ARGS__ << "]:",     \
   dbg_impl::out(__VA_ARGS__))
#else
#define dbg(...) 42
#endif
