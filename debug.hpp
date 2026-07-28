#pragma once
// IWYU pragma: always_keep
// clangd's include-cleaner would otherwise flag `#include "debug.hpp"` as an
// unused include in any file that doesn't happen to call dbg() this session

// dbg(...) pretty-prints its arguments to stderr as `name = value` pairs so
// it's obvious which value is which. Each line starts with two color-coded,
// width-aligned columns: `+<delta>ms` (gray, time since the previous dbg call)
// and `L<line>` (cyan); each argument's name is red, its value default-colored.
// With no arguments, dbg() prints just the prefix as a "reached here"
// checkpoint. Colors are emitted only when stderr is a tty. Active only under
// -DLOCAL; otherwise it expands to 42, so the same file still builds on judges
// that don't ship this header.
//
// stringify() dispatches at compile time (C++20 concepts + if constexpr) and
// handles: arithmetic, __int128, char, string/string_view, bool, any
// range/container (nested, maps, set, span, ...), stack/queue/priority_queue,
// pair, tuple/array of any arity, optional, variant, expected (C++23), bitset,
// and anything with an operator<<. Large ranges are truncated. 2D grids (a
// range of ranges, including vector<string>) are laid out one row per line.

#include <bits/stdc++.h>
#include <unistd.h>  // isatty (not part of bits/stdc++.h)

namespace dbg_impl {
using std::string;

template <class T>
string stringify(const T& x);

// color() is defined below but used inside stringify's grid branch (row-index
// labels); forward-declare it and its enum so name lookup succeeds there
enum col { reset, gray, cyan, red };
inline const char* color(col c);

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
template <class> inline constexpr bool is_adapter_v = false;
template <class T, class C> inline constexpr bool is_adapter_v<std::stack<T, C>> = true;
template <class T, class C> inline constexpr bool is_adapter_v<std::queue<T, C>> = true;
template <class T, class C, class Cmp>
inline constexpr bool is_adapter_v<std::priority_queue<T, C, Cmp>> = true;
#ifdef __cpp_lib_expected
template <class> inline constexpr bool is_expected_v = false;
template <class T, class E> inline constexpr bool is_expected_v<std::expected<T, E>> = true;
#endif

template <class T>
concept Streamable = requires(std::ostream& os, const T& x) { os << x; };

// a grid is a range whose elements are themselves ranges (e.g.
// vector<vector<int>>, vector<string>); printed one row per line
template <class T>
concept Grid = std::ranges::range<T> &&
    std::ranges::range<std::ranges::range_value_t<T>>;

// cap on how many elements of a range we print before eliding the rest
inline constexpr size_t max_elems = 512;

// stack/queue/priority_queue keep their storage in a protected member `c`;
// reach it through a subclass that re-exposes the member pointer
template <class Adapter>
const auto& underlying(const Adapter& a) {
  struct hack : Adapter {
    static const auto& get(const Adapter& a) { return a.*&hack::c; }
  };
  return hack::get(a);
}

template <class T>
string stringify(const T& x) {
  if constexpr (std::is_same_v<T, bool>) {
    return x ? "true" : "false";
  } else if constexpr (std::is_same_v<T, char>) {
    return "'" + string(1, x) + "'";
  } else if constexpr (std::is_same_v<T, std::monostate>) {
    return "monostate";
    // __int128 has no std::to_string overload; build the digits by hand.
    // the pragma silences -Wpedantic's "ISO C++ does not support __int128"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
  } else if constexpr (std::is_same_v<T, __int128> ||
                       std::is_same_v<T, unsigned __int128>) {
    if (x == 0) return "0";
    bool neg = false;
    unsigned __int128 u;
    if constexpr (std::is_same_v<T, __int128>) {
      neg = x < 0;
      u = neg ? -static_cast<unsigned __int128>(x) : x;
    } else {
      u = x;
    }
    string res;
    while (u) {
      res += char('0' + int(u % 10));
      u /= 10;
    }
    if (neg) res += '-';
    std::reverse(res.begin(), res.end());
    return res;
#pragma GCC diagnostic pop
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
  } else if constexpr (is_adapter_v<T>) {
    return stringify(underlying(x));
  } else if constexpr (Grid<T>) {
    // each row is printed on its own line, prefixed with its right-aligned
    // index in subscript form (`[<i>]`, gray) so it reads like grid[i] —
    // invaluable for adjacency lists where the index is the node id. char grids
    // (vector<string>) print raw and tight; value grids get each cell
    // right-aligned to the widest cell with a leading space
    using Cell = std::ranges::range_value_t<std::ranges::range_value_t<T>>;
    size_t rows = std::ranges::distance(x);
    int idx_w = int(std::to_string(rows ? rows - 1 : 0).size());
    auto label = [&](size_t i) {
      std::ostringstream os;
      os << color(gray) << '[' << std::setw(idx_w) << std::right << i << ']'
         << color(reset) << ' ';
      return os.str();
    };
    if constexpr (std::is_same_v<Cell, char>) {
      string res;
      size_t i = 0;
      for (const auto& row : x) {
        res += '\n' + label(i++);
        for (char e : row) res += e;
      }
      return res;
    } else {
      std::vector<std::vector<string>> cells;
      size_t w = 0;
      for (const auto& row : x) {
        cells.emplace_back();
        for (const auto& e : row) {
          cells.back().push_back(stringify(e));
          w = std::max(w, cells.back().back().size());
        }
      }
      string res;
      size_t i = 0;
      for (const auto& row : cells) {
        res += '\n' + label(i++);
        for (const auto& c : row) res += string(w - c.size() + 1, ' ') + c;
      }
      return res;
    }
  } else if constexpr (std::ranges::range<T>) {
    string res = "{";
    bool sep = false;
    size_t n = 0;
    for (const auto& e : x) {
      if (n++ == max_elems) {
        res += ", ...";
        if constexpr (std::ranges::sized_range<T>)
          res += " (+" + std::to_string(std::ranges::size(x) - max_elems) + " more)";
        break;
      }
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

// ANSI colors, but only when stderr is a terminal; piped/redirected output
// stays plain so files never get escape-code garbage. Each part of a debug
// line gets its own color so time / line / expression are easy to tell apart.
// (col and this signature are forward-declared near the top of the namespace.)
inline const char* color(col c) {
  static const bool tty = isatty(fileno(stderr));
  if (!tty) return "";
  switch (c) {
    case gray: return "\033[90m";
    case cyan: return "\033[36m";
    case red: return "\033[31m";
    default: return "\033[0m";
  }
}

// delta ms since the previous dbg() call (the first call measures from here)
inline long long delta_ms() {
  static auto last = std::chrono::steady_clock::now();
  auto now = std::chrono::steady_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count();
  last = now;
  return ms;
}

// widths of the time and line columns; their sum is the fixed gutter that
// continuation lines (grid rows) are indented to so they stack in one column
inline constexpr int time_w = 8, line_w = 6, gutter = time_w + line_w;

// print the aligned, colored prefix: +<ms> (gray) | L<line> (cyan)
inline void head(int line) {
  std::cout.flush();  // interleave with buffered cout (see macro note below)
  std::ostringstream t;
  t << '+' << delta_ms() << "ms";
  std::ostringstream l;
  l << 'L' << line;
  std::cerr << color(gray) << std::setw(time_w) << std::left << t.str()
            << color(cyan) << std::setw(line_w) << std::left << l.str();
}

// push any embedded newline (grids print one row per line) into the fixed
// gutter so multi-line values line up in a consistent left column
inline string indent(string s) {
  string pad = "\n" + string(gutter, ' ');
  for (size_t p = s.find('\n'); p != string::npos; p = s.find('\n', p + pad.size()))
    s.replace(p, 1, pad);
  return s;
}

// split #__VA_ARGS__ into the individual argument names, breaking only on
// top-level commas so args like make_pair(1, 2) or v[{1, 2}] stay intact
inline std::vector<string> arg_names(const char* expr) {
  std::vector<string> names;
  string cur;
  int depth = 0;
  char quote = 0;
  for (const char* s = expr; *s; s++) {
    char ch = *s;
    if (quote) {
      cur += ch;
      if (ch == quote && s[-1] != '\\') quote = 0;
    } else if (ch == '"' || ch == '\'') {
      quote = ch;
      cur += ch;
    } else if (ch == '(' || ch == '[' || ch == '{' || ch == '<') {
      depth++;
      cur += ch;
    } else if (ch == ')' || ch == ']' || ch == '}' || ch == '>') {
      depth--;
      cur += ch;
    } else if (ch == ',' && depth == 0) {
      names.push_back(cur);
      cur.clear();
    } else {
      cur += ch;
    }
  }
  names.push_back(cur);
  for (auto& n : names) {  // trim surrounding whitespace
    size_t a = n.find_first_not_of(" \t"), b = n.find_last_not_of(" \t");
    n = (a == string::npos) ? "" : n.substr(a, b - a + 1);
  }
  return names;
}

// no-arg dbg(): just terminate the prefix line so it reads as a "reached here"
// checkpoint (only the +<ms> and L<line> columns, no values)
inline void out(const char*) { std::cerr << color(reset) << std::endl; }

// print each argument as `name = value`, name in red, value in default color,
// so it's obvious which value belongs to which expression
template <class... Ts>
void out(const char* expr, const Ts&... xs) {
  auto names = arg_names(expr);
  size_t i = 0;
  // read i for the separator/name, then bump it as a separate statement so
  // there's a sequence point between the read and the write (no UB)
  auto one = [&](const auto& x) {
    std::cerr << (i ? ", " : " ") << color(red) << names[i] << color(reset)
              << " = " << indent(stringify(x));
    i++;
  };
  (one(xs), ...);
  std::cerr << color(reset) << std::endl;
}

}  // namespace dbg_impl

#ifdef LOCAL
// flush cout first: with sync_with_stdio(0) cout is buffered until exit while
// cerr is unbuffered, so without this every debug line prints before any real
// output instead of interleaving in program order (handled in head())
#define dbg(...)                                     \
  (dbg_impl::head(__LINE__),                         \
   dbg_impl::out(#__VA_ARGS__ __VA_OPT__(, ) __VA_ARGS__))
#else
#define dbg(...) 42
#endif
