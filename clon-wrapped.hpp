#ifndef __clon_clon_wrapped_hpp__
#define __clon_clon_wrapped_hpp__

#include <string_view>
#include <string>
#include <variant>
#include <vector>
#include <limits>
#include <array>
#include <stdexcept>

#include "format.hpp"

namespace clon
{
  enum struct clon_type : unsigned
  {
    none = 0,
    boolean = 1,
    number = 2,
    string = 3,
    list = 4
  };

  struct list_tag
  {
  };

  template <typename char_t>
  using string = std::basic_string_view<char_t>;
  using none = std::monostate;
  using number = double;
  using boolean = bool;
  using list = list_tag;

  template <typename char_t>
  using value = std::variant<none, boolean, number, string<char_t>, list>;

  template <typename char_t, typename type_t>
  concept possible_value =
      std::same_as<type_t, boolean> or
      std::same_as<type_t, number> or
      std::same_as<type_t, string<char_t>> or
      std::same_as<type_t, list>;

  template <std::integral t>
  constexpr t maxof = std::numeric_limits<t>::max();

  constexpr std::size_t no_next = maxof<std::size_t>;
  constexpr std::size_t no_child = maxof<std::size_t>;
  constexpr std::size_t no_root = maxof<std::size_t>;

  template <typename char_t>
  number to_number(std::basic_string_view<char_t> v)
  {
    number n = 0;

    for (const char_t &c : v)
      n = n * 10 + (c - '0');

    return n;
  }

  template <typename char_t>
  std::size_t to_integer(std::basic_string_view<char_t> v)
  {
    std::size_t n = 0;

    for (const char_t &c : v)
      n = n * 10 + (c - '0');

    return n;
  }

  template <typename char_t>
  class node
  {
  public:
    node() = default;
    explicit node(
        clon_type type,
        std::basic_string_view<char_t> _name,
        std::basic_string_view<char_t> _valv)
        : __name(_name), __valv(_valv)
    {
      using cl = clon_type;

      switch (type)
      {
      case cl::boolean:
        __val = __valv == "true";
        break;
      case cl::number:
        __val = to_number(__valv);
        break;
      case cl::string:
        __val = __valv;
        break;
      case cl::list:
        __val = list();
        break;
      case cl::none:
        __val = std::monostate{};
        break;
      }
    }

  public:
    const clon_type type() const
    {
      return static_cast<clon_type>(__val.index());
    }

    template <typename type_t>
    requires possible_value<char_t, type_t> const bool is_() const
    {
      using cl = clon_type;

      if constexpr (std::is_same_v<type_t, boolean>)
        return type() == cl::boolean;
      if constexpr (std::is_same_v<type_t, list>)
        return type() == cl::list;
      if constexpr (std::is_same_v<type_t, string<char_t>>)
        return type() == cl::string;
      if constexpr (std::is_same_v<type_t, number>)
        return type() == cl::number;
      if constexpr (std::is_same_v<type_t, none>)
        return type() == cl::none;
    }

    template <typename type_t>
    requires possible_value<char_t, type_t> const type_t &as_() const
    {
      return std::get<type_t>(__val);
    }

    const std::basic_string_view<char_t> &name() const { return __name; }
    const std::size_t &child() const { return __child; }
    const std::size_t &next() const { return __next; }
    const std::basic_string_view<char_t> &valv() const { return __valv; }

  public:
    const std::size_t &update_child(std::size_t _child)
    {
      return __child = _child;
    }

    const std::size_t &update_next(std::size_t _next)
    {
      return __next = _next;
    }

  private:
    std::basic_string_view<char_t> __name;
    value<char_t> __val;
    std::basic_string_view<char_t> __valv;
    std::size_t __next = no_next;
    std::size_t __child = no_child;
  };

  enum class symbol_type : int
  {
    eos,
    blank,
    letter_f,
    letter_t,
    lower,
    dquote,
    digit,
    lpar,
    rpar,
    other
  };

  constexpr std::array<symbol_type, 128> ascii_build()
  {
    using sb = symbol_type;

    std::array<sb, 128> table;

    for (sb &sb : table)
      sb = sb::other;

    for (char l = 'a'; l <= 'z'; ++l)
      table[l] = sb::lower;

    for (char l = '0'; l <= '9'; ++l)
      table[l] = sb::digit;

    table['f'] = sb::letter_f;
    table['t'] = sb::letter_t;
    table['"'] = sb::dquote;
    table['('] = sb::lpar;
    table[')'] = sb::rpar;
    table[' '] = sb::blank;
    table['\n'] = sb::blank;
    table['\r'] = sb::blank;
    table['\t'] = sb::blank;
    table['\0'] = sb::eos;

    return table;
  }

  constexpr std::array<symbol_type, 128> ascii_to_sb = ascii_build();

  template <typename char_t>
  class parser final
  {
  public:
    explicit parser(
        std::vector<node<char_t>> &nodes,
        std::basic_string_view<char_t> v)
        : __nodes(nodes),
          __data(v) {}

  public:
    void parse_node()
    {
      open_node();

      std::basic_string_view<char_t> &&n(scan_name());
      ignore_blanks();

      switch (symbol())
      {
      case sb::letter_f:
      case sb::letter_t:
        __nodes.emplace_back(node<char_t>(cl::boolean, n, scan_boolean()));
        break;
      case sb::dquote:
        __nodes.emplace_back(node<char_t>(cl::string, n, scan_string()));
        break;
      case sb::digit:
        __nodes.emplace_back(node<char_t>(cl::number, n, scan_number()));
        break;
      case sb::lpar:
        __nodes.emplace_back(node<char_t>(cl::list, n, scan_list()));
        __nodes.back().update_child(__nodes.size());
        parse_list();
        break;
      default:
        break;
      }

      close_node();
    }

  private:
    using sb = symbol_type;
    using cl = clon_type;

    void parse_list()
    {
      ignore_blanks();
      std::size_t index = __nodes.size();
      std::size_t count = 0;

      while (symbol() == sb::lpar)
      {
        if (count > 0)
          index = __nodes[index].update_next(__nodes.size());

        parse_node();
        ignore_blanks();
        count++;
      }
    }

    symbol_type symbol()
    {
      if (__index < __data.size())
        return __data[__index] > 127
                   ? sb::other
                   : ascii_to_sb[__data[__index]];
      else
        return sb::eos;
    }

    std::basic_string_view<char_t> scan_name()
    {
      ignore_blanks();

      sb s = symbol();

      if (s != sb::lower and
          s != sb::letter_f and
          s != sb::letter_t)
        handle_error_expecting("[a-z]");

      while (s == sb::lower or s == sb::letter_f or s == sb::letter_t)
      {
        advance();
        s = symbol();
      }

      return extract();
    }

    std::basic_string_view<char_t> scan_boolean()
    {
      ignore_blanks();

      if (starts_with("true"))
        advance(4);
      else if (starts_with("false"))
        advance(5);
      else
        handle_error_expecting("'true' or 'false'");

      return extract();
    }

    std::basic_string_view<char_t> scan_string()
    {
      ignore_blanks();

      if (symbol() == sb::dquote)
      {
        advance();

        while (symbol() != sb::dquote)
          advance();

        if (symbol() == sb::dquote)
          advance();
        else
          handle_error_expecting("'\"'");
      }
      else
        handle_error_expecting("'\"'");

      return extract();
    }

    std::basic_string_view<char_t> scan_number()
    {
      ignore_blanks();

      if (symbol() != sb::digit)
        handle_error_expecting("[0-9]");

      while (symbol() == sb::digit)
        advance();

      return extract();
    }

    std::basic_string_view<char_t> scan_list()
    {
      return {};
    }

    void open_node()
    {
      ignore_blanks();

      if (symbol() == sb::lpar)
      {
        advance();
        ignore();
      }
      else
        handle_error_expecting("'('");
    }

    void close_node()
    {
      ignore_blanks();

      if (symbol() == sb::rpar)
      {
        advance();
        ignore();
      }
      else
        handle_error_expecting("')'");
    }

    void ignore_blanks()
    {
      while (symbol() == sb::blank)
        advance();

      ignore();
    }

    bool starts_with(
        std::basic_string_view<char_t> v)
    {
      return __index < __data.size() and __data.substr(__index).starts_with(v);
    }

    void advance(std::size_t step = 1)
    {
      if (__index + step <= __data.size())
        __index += step;
    }

    std::basic_string_view<char_t> extract()
    {
      std::basic_string_view<char_t> tk(&__data[__prev], __index - __prev);
      __prev = __index;
      return tk;
    }

    void ignore()
    {
      __prev = __index;
    }

    template <std::size_t n>
    void handle_error_expecting(const char (&expected)[n])
    {
      throw std::runtime_error(
          clon::fmt::format(
              "scanning : expected {} at index {}", expected, __index));
    }

  private:
    std::vector<node<char_t>> &__nodes;
    std::basic_string_view<char_t> __data;
    std::size_t __index = 0;
    std::size_t __prev = 0;
  };

  template <
      typename char_t>
  class root_view;

  template <typename char_t>
  class root final
  {
  public:
    explicit root(std::basic_string_view<char_t> v)
        : buff(v.begin(), v.end())
    {
      nodes.reserve(std::count(buff.begin(), buff.end(), '('));
      std::basic_string_view<char_t> vbuff(buff.begin(), buff.end());

      try
      {
        parser<char_t>(nodes, vbuff).parse_node();
      }
      catch (const std::exception &e)
      {
        nodes.clear();
        throw e;
      }
    }

  public:
    static const node<char_t> &undefined()
    {
      static node<char_t> undef;
      return undef;
    }

    const node<char_t> &get(std::basic_string_view<char_t> pth) const
    {
      return undefined();
    }

    const node<char_t> &operator[](std::basic_string_view<char_t> pth) const
    {
      return get(pth);
    }

    bool parsed() const
    {
      return not nodes.empty();
    }

    root_view<char_t>
    view(const std::size_t &index = 0) const
    {
      return root_view<char_t>(*this, index);
    }

    std::basic_string<char_t> to_string() const
    {
      return clon::fmt::format("{}", view());
    }

    const node<char_t> &at(
        const std::size_t &index) const
    {
      return index >= nodes.size() ? undefined() : nodes[index];
    }

    std::size_t buffer_size() const { return buff.size(); }
    std::size_t nodes_size() const { return nodes.size(); }

  private:
    std::vector<char_t> buff;
    std::vector<node<char_t>> nodes;
  };

  template <
      typename char_t>
  class root_view
  {
  public:
    explicit root_view(
        const root<char_t> &r,
        const std::size_t &index)
        : __r(r),
          __index(index >= r.nodes_size() ? no_root : index) {}

  public:
    root_view begin() const { return root_view(__r, __index); }
    root_view end() const { return root_view(__r, no_next); }

    std::pair<const node<char_t> &, std::size_t>
    operator*()
    {
      return {__r.at(__index), __index};
    }

    root_view &operator++()
    {
      __index = __r.at(__index).next();
      return *this;
    }

    root_view operator++(int)
    {
      root_view tmp(*this);
      ++(*this);
      return tmp;
    }

    friend bool operator==(
        const root_view &a,
        const root_view &b)
    {
      return &a.__r == &b.__r and
             a.__index == b.__index;
    }

    friend bool operator!=(
        const root_view &a,
        const root_view &b)
    {
      return not(a == b);
    }

    friend std::size_t length_of(
        const root_view &r)
    {
      return r.__r.parsed() ? r.__r.buffer_size() : 0;
    }

    friend void format_of(
        clon::fmt::formatter_context<char_t> &ctx,
        const root_view &rf)
    {
      namespace fmt = ::clon::fmt;
      const node<char_t> &n = rf.__r.at(rf.__index);

      switch (n.type())
      {
      case clon_type::boolean:
      case clon_type::number:
      case clon_type::string:
        fmt::format_into(ctx, "({} {})", n.name(), n.valv());
        break;
      case clon_type::list:
        fmt::format_into(ctx, "({} ", n.name());
        for (auto &&ni : rf.__r.view(n.child()))
          format_of(ctx, rf.__r.view(ni.second));
        fmt::format_into(ctx, ")");
        break;
      case clon_type::none:
        break;
      }
    }

  private:
    const root<char_t> &__r;
    std::size_t __index = no_root;
  };

  constexpr std::size_t path_max = maxof<std::size_t>;

  template <typename char_t>
  struct path
  {
    std::basic_string_view<char_t> name;
    std::size_t min;
    std::size_t max;
  };

  template <typename char_t>
  class path_parser
  {
  public:
    explicit path_parser(std::basic_string_view<char_t> _path)
        : __data(_path) {}

  public:
    path<char_t> parse()
    {
      path<char_t> p;
      p.name = scan_name();

      if (scan_colon())
      {
        auto &&[min, max] = scan_interval();

        p.max = max;
        p.min = min;
      }

      return p;
    }

    bool scan_colon()
    {
      if (__data[__index] == ':')
      {
        ignore();
        return true;
      }
      return false;
    }

    std::pair<std::size_t, std::size_t> scan_interval()
    {
      if (__data[__index] == '*')
        return {path_max, path_max};
      else
      {
        std::size_t &&min = to_integer(scan_number());
        return {min, min};
      }
    }

    std::basic_string_view<char_t> scan_name()
    {
      if ('a' <= __data[__index] and __data[__index] <= 'z')
        handle_error_expecting("[a-z]");

      while ('a' <= __data[__index] and __data[__index] <= 'z')
        advance();

      return extract();
    }

    std::basic_string_view<char_t> scan_number()
    {
      if ('0' <= __data[__index] and __data[__index] <= '9')
        handle_error_expecting("[0-9]");

      while ('0' <= __data[__index] and __data[__index] <= '9')
        advance();

      return extract();
    }

  private:
    void advance(std::size_t step = 1)
    {
      if (__index + step <= __data.size())
        __index += step;
    }

    std::basic_string_view<char_t> extract()
    {
      std::basic_string_view<char_t> tk(&__data[__prev], __index - __prev);
      __prev = __index;
      return tk;
    }

    void ignore()
    {
      __prev = __index;
    }

    template <std::size_t n>
    void handle_error_expecting(const char (&expected)[n])
    {
      throw std::runtime_error(
          clon::fmt::format(
              "scanning : expected {} at index {}", expected, __index));
    }

  private:
    std::basic_string_view<char_t> __data;
    std::size_t __index = 0;
    std::size_t __prev = 0;
  };

  template <typename char_t>
  void explore(
      const path<char_t> &pth,
      const root<char_t> &r)
  {
  }

  template <typename char_t>
  void explore(
      std::basic_string_view<char_t> pth,
      const root<char_t> &r)
  {
    return explore(path_parser<char_t>(pth).parse(), r);
  }

} // namespace clon

#endif