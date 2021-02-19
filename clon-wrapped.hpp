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

  template <typename char_t>
  struct node;

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

  constexpr std::size_t no_next = std::numeric_limits<std::size_t>::max();
  constexpr std::size_t no_child = std::numeric_limits<std::size_t>::max();

  template <typename char_t>
  number to_number(std::basic_string_view<char_t> v)
  {
    number n = 0;

    for (const char_t &c : v)
      n = n * 10 + (c - '0');

    return n;
  }

  template <typename char_t>
  class node
  {
  public:
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

    const std::basic_string_view<char_t> &name() const
    {
      return __name;
    }

    const std::size_t &child() const
    {
      return __child;
    }

    const std::size_t &next() const
    {
      return __next;
    }

    const std::basic_string_view<char_t> valv() const
    {
      return __valv;
    }

  public:
    std::size_t update_child(std::size_t _child)
    {
      return __child = _child;
    }

    std::size_t update_next(std::size_t _next)
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
  class scanner final
  {
  public:
    explicit scanner(
        std::basic_string_view<char_t> v)
        : __data(v) {}

  public:
    symbol_type symbol()
    {
      if (not closed())
        if (ch() > 127)
          return sb::other;
        else
          return ascii_to_sb[ch()];
      else
        return sb::eos;
    }

    std::basic_string_view<char_t> name()
    {
      ignore_blanks();
      if (symbol() != sb::lower and
          symbol() != sb::letter_f and
          symbol() != sb::letter_t)
        throw std::runtime_error(
          clon::fmt::format(
            "name scanning : expected char in [a-z] at index {}", __index));

      while (symbol() == sb::lower or
             symbol() == sb::letter_f or
             symbol() == sb::letter_t)
        advance();

      return extract();
    }

    std::basic_string_view<char_t> boolean()
    {
      ignore_blanks();

      if (starts_with("true"))
        advance(4);
      else if (starts_with("false"))
        advance(5);
      else
        throw std::runtime_error(
            clon::fmt::format(
              "boolean scanning : expected 'true' or 'false' at index {}", __index));

      return extract();
    }

    std::basic_string_view<char_t> string()
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
          throw std::runtime_error(
              clon::fmt::format(
                "string scanning : expected '\"' at index {}", __index));
      }
      else
        throw std::runtime_error(
            clon::fmt::format(
              "string scanning : expected '\"' at index {}", __index));

      return extract();
    }

    std::basic_string_view<char_t> number()
    {
      ignore_blanks();

      if (symbol() != sb::digit)
        throw std::runtime_error(
            clon::fmt::format(
              "number scanning : expected char in [0-9] at index {}", __index));

      while (symbol() == sb::digit)
        advance();

      return extract();
    }

    void open_node()
    {
      ignore_blanks();

      if (symbol() == sb::lpar)
        ignore_symbol();
      else
        throw std::runtime_error(
            clon::fmt::format(
              "lpar scanning : expected char '(' at index {}", __index));
    }

    void close_node()
    {
      ignore_blanks();

      if (symbol() == sb::rpar)
        ignore_symbol();
      else
        throw std::runtime_error(
            clon::fmt::format(
              "lpar scanning : expected char '(' at index {}", __index));
    }

    void ignore_blanks()
    {
      while (symbol() == sb::blank)
        advance();

      ignore();
    }

    void ignore_symbol()
    {
      advance();
      ignore();
    }

  private:
    const char_t &ch()
    {
      return __data[__index];
    }

    bool closed()
    {
      return __index >= __data.size();
    }

    bool currently_is(
        const char_t &c)
    {
      return ch() == c;
    }

    bool currently_in(
        const char_t &mn,
        const char_t &mx)
    {
      return mn <= ch() and ch() <= mx;
    }

    template <typename... chars_t>
    bool currently_oneof(
        const char_t &h,
        const chars_t &...t)
    {
      return ((ch() == t) or ... or (ch() == h));
    }

    bool starts_with(
        std::basic_string_view<char_t> v)
    {
      return not closed() and __data.substr(__index).starts_with(v);
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

  private:
    using sb = symbol_type;

    std::basic_string_view<char_t> __data;
    std::size_t __index = 0;
    std::size_t __prev = 0;
  };

  template <typename char_t>
  class parser final
  {
  public:
    explicit parser(
        std::vector<node<char_t>> &nodes,
        std::basic_string_view<char_t> v)
        : nodes(nodes), scan(v) {}

  public:
    void parse_node()
    {
      scan.open_node();

      std::basic_string_view<char_t> &&name(scan.name());
      scan.ignore_blanks();

      switch (scan.symbol())
      {
      case sb::letter_f:
      case sb::letter_t:
        nodes.emplace_back(node<char_t>(cl::boolean, name, scan.boolean()));
        break;
      case sb::dquote:
        nodes.emplace_back(node<char_t>(cl::string, name, scan.string()));
        break;
      case sb::digit:
        nodes.emplace_back(node<char_t>(cl::number, name, scan.number()));
        break;
      case sb::lpar:
      {
        nodes.emplace_back(node<char_t>(cl::list, name, std::basic_string_view<char_t>()));
        nodes.back().update_child(nodes.size());
        parse_list();
        break;
      }
      default:
        break;
      }

      scan.close_node();
    }

  private:
    using sb = symbol_type;
    using cl = clon_type;

    void parse_list()
    {
      scan.ignore_blanks();
      std::size_t index = nodes.size();
      std::size_t count = 0;

      while (scan.symbol() == sb::lpar)
      {
        if (count > 0)
          index = nodes[index].update_next(nodes.size());

        parse_node();
        scan.ignore_blanks();
        count++;
      }
    }

  private:
    std::vector<node<char_t>> &nodes;
    scanner<char_t> scan;
  };

  template <typename char_t,
            typename parser_t = parser<char_t>>
  class root final
  {
  public:
    explicit root(std::basic_string_view<char_t> v)
        : buff(v.begin(), v.end())
    {
      nodes.reserve(std::count(buff.begin(), buff.end(), '('));
      std::basic_string_view<char_t> vbuff(buff.begin(), buff.end());
      parser_t(nodes, vbuff).parse_node();
    }

  public:
    static const node<char_t> &undefined()
    {
      static node<char_t> undef;
      return undef;
    }

    const std::basic_string<char_t> to_string() const
    {
      std::basic_string<char_t> s;
      s.reserve(buff.size());
      return s;
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

  public:
    std::vector<char_t> buff;
    std::vector<node<char_t>> nodes;
  };

} // namespace clon

#endif