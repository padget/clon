#ifndef __clon_clon_wrapped_hpp__
#define __clon_clon_wrapped_hpp__

#include <string_view>
#include <string>
#include <variant>
#include <vector>
#include <span>

namespace clon
{
  template <typename char_t>
  using sview = std::basic_string_view<char_t>;

  enum struct clon_type : unsigned
  {
    none = 0,
    boolean = 1,
    number = 2,
    string = 3,
    object = 4
  };

  template <typename char_t>
  struct node;

  template <typename char_t, typename type_t>
  struct wrapper
  {
    sview<char_t> valv;
  };

  template <typename char_t>
  using number = wrapper<char_t, double>;

  template <typename char_t>
  using string = wrapper<char_t, sview<char_t>>;

  template <typename char_t>
  using boolean = wrapper<char_t, bool>;

  template <typename char_t>
  using object = wrapper<char_t, std::size_t>;

  using none = std::monostate;

  template <typename char_t>
  using value = std::variant<
      none,
      boolean<char_t>,
      number<char_t>,
      string<char_t>,
      object<char_t>>;

  template <typename char_t, typename type_t>
  concept possible_value =
      std::same_as<type_t, boolean<char_t>> or
      std::same_as<type_t, number<char_t>> or
      std::same_as<type_t, string<char_t>> or
      std::same_as<type_t, object<char_t>>;

  template <typename char_t>
  class node
  {
  public:
    template <possible_value<char_t> value_t>
    explicit node(
        sview<char_t> _name,
        const value_t &_val,
        std::size_t _next,
        std::size_t _parent)
        : name(_name),
          val(_val),
          next(_next),
          parent(_parent) {}

  public:
    const clon_type type() const noexcept
    {
      return static_cast<clon_type>(val.index());
    }

    template <possible_value<char_t> type_t>
    const bool is_() const
    {
      if constexpr (std::is_same_v<type_t, boolean<char_t>>)
        return type() == clon_type::boolean;
      if constexpr (std::is_same_v<type_t, object<char_t>>)
        return type() == clon_type::object;
      if constexpr (std::is_same_v<type_t, string<char_t>>)
        return type() == clon_type::string;
      if constexpr (std::is_same_v<type_t, number<char_t>>)
        return type() == clon_type::number;
      if constexpr (std::is_same_v<type_t, none>)
        return type() == clon_type::none;
    }

    template <possible_value<char_t> type_t>
    const type_t &as_() const
    {
      return std::get<type_t>(val);
    }

  private:
    sview<char_t> name;
    value<char_t> val;
    std::size_t next;
    std::size_t parent;
  };

  template <typename char_t, typename... chars_t>
  bool is_one_of(char_t c, char_t f, chars_t... cs)
  {
    return ((c == cs) or ... or (c == f));
  }

  template <typename iterator>
  iterator omit_blanks(iterator b, iterator e)
  {
    while (b != e and is_one_of(*b, ' ', '\t', '\n', '\r'))
      ++b;

    return b;
  }

  template <typename iterator>
  clon_type next_could_be(iterator b, iterator e)
  {
    clon_type type(clon_type::none);

    if (b != e)
    {
      const auto &c = *b;

      if (c == '"')
        type = clon_type::string;
      else if (c == '(')
        type = clon_type::object;
      else if (c == 't' or c == 'f')
        type = clon_type::boolean;
      else if ('0' <= c <= '9')
        type = clon_type::number;
    }

    return type;
  }

  enum class symbol_type : int
  {
    eos,
    blank,
    letter_f,
    letter_t,
    dquote,
    digit,
    lpar,
    rpar,
    other
  };

  template <typename char_t>
  struct token
  {
    std::basic_string_view<char_t> v;
  };

  template <typename char_t>
  class scanner_context
  {
    std::basic_string_view<char_t> data;
    std::size_t index = 0;
    std::size_t prev = 0;

  public:
    explicit scanner_context(
        std::basic_string_view<char_t> d)
        : data(d) {}

  public:
    const char_t &ch() const
    {
      return data[index];
    }

    const bool closed() const
    {
      return index >= data.size();
    }

    const bool currently_is(
        const char_t &c) const
    {
      return ch() == c;
    }

    const bool currently_is(
        const char_t &mn,
        const char_t &mx) const
    {
      return mn <= ch() and ch() <= mx;
    }

    template <typename... chars_t>
    const bool currently_oneof(
        const char_t &h,
        const chars_t &...t) const
    {
      return ((ch() == t) or ... or (ch() == h));
    }

    const bool starts_with(
        std::basic_string_view<char_t> v) const
    {
      return data.substr(index).starts_with(v);
    }

    void advance(std::size_t step = 1)
    {
      if (index + step <= data.size())
        index += step;
    }

    token<char_t> extract()
    {
      std::basic_string_view<char_t> tk(&data[prev], index - prev);
      prev = index;
      return tk;
    }

    void ignore()
    {
      prev = index;
    }
  };

  template <typename char_t>
  class scanner
  {
    scanner_context<char_t> ctx;

  public:
    explicit scanner(
        std::basic_string_view<char_t> v)
        : ctx(v) {}

  public:
    symbol_type symbol()
    {
      using sb = symbol_type;

      if (not ctx.closed())
      {
        if (ctx.currently_is('('))
          return sb::lpar;
        else if (ctx.currently_is(')'))
          return sb::rpar;
        else if (ctx.currently_is('"'))
          return sb::dquote;
        else if (ctx.currently_is('t'))
          return sb::letter_t;
        else if (ctx.currently_is('f'))
          return sb::letter_f;
        else if (ctx.currently_is('0', '9'))
          return sb::digit;
        else if (ctx.currently_oneof(' ', '\n', '\r', '\t'))
          return sb::blank;
        else
          return sb::other;
      }
      else
        return sb::eos;
    }

    token<char_t> name()
    {
      while (not ctx.closed() and
             ctx.currently_is('a', 'z'))
        ctx.advance();

      return ctx.extract();
    }

    token<char_t> boolean()
    {
      if (not ctx.closed() and
          ctx.starts_with("true"))
        ctx.advance(4);
      else if (not ctx.closed() and
               ctx.starts_with("false"))
        ctx.advance(5);

      return ctx.extract();
    }

    token<char_t> string()
    {
      if (ctx.currently_is('"'))
      {
        ctx.advance();

        while (not ctx.closed() and
               not ctx.currently_is('"'))
          ctx.advance();

        if (not ctx.closed() and
            ctx.currently_is('"'))
          ctx.advance();
      }

      return ctx.extract();
    }

    token<char_t> number()
    {
      while (not ctx.closed() and
             ctx.currently_is('0', '9'))
        ctx.advance();

      return ctx.extract();
    }

    void ignore_blanks()
    {
      while (not ctx.closed() and
             ctx.currently_oneof(' ', '\n', '\r', '\t'))
        ctx.advance();

      ctx.ignore();
    }

    void ignore_symbol()
    {
      ctx.advance();
      ctx.ignore();
    }
  };

  template <typename char_t>
  struct parser_context
  {
    std::vector<node<char_t>> &nodes;
    scanner<char_t> scan;

  public:
    explicit parser_context(
        std::vector<node<char_t>> &_nodes,
        std::basic_string_view<char_t> v)
        : nodes(_nodes), scan(v) {}

  public:
    void open_object()
    {
      scan.ignore_blanks();

      if (scan.symbol() == symbol_type::lpar)
        scan.ignore_symbol();
    }

    void close_object()
    {
      scan.ignore_blanks();

      if (scan.symbol() == symbol_type::rpar)
        scan.ignore_symbol();
    }

    token<char_t> object_name()
    {
      return scan.name();
    }
  };

  template <typename char_t>
  struct parser final 
  {
  private:
    parser_context<char_t> ctx;

  private:
    using parent_index = std::size_t;

  public:
    explicit parser(
        std::vector<node<char_t>> &nodes,
        std::basic_string_view<char_t> v)
        : ctx(nodes, v) {}

  public:
    void object(parent_index parent)
    {
      ctx.open_object();

      token<char_t> &&name(ctx.scan.name());

      // TODO HERE
 
      ctx.close_object();
    }
  };

  template <typename char_t,
            typename parser_t = parser<char_t>>
  class root final
  {
  public:
    explicit root(sview<char_t> v) noexcept
        : buff(v.begin(), v.end())
    {
      nodes.reserve(std::count(buff.begin(), buff.end(), '('));
      parser_t{}.parse(buff, nodes);
    }

  public:
    static const node<char_t> &undefined() noexcept
    {
      static node<char_t> undef;
      return undef;
    }

    const std::basic_string<char_t> to_string() const noexcept
    {
      std::basic_string<char_t> s;
      s.reserve(buff.size());
      return s;
    }

    const node<char_t> &get(sview<char_t> pth) const noexcept
    {
      return undefined();
    }

    const node<char_t> &operator[](sview<char_t> pth) const noexcept
    {
      return get(pth);
    }

    bool parsed() const noexcept
    {
      return not nodes.empty();
    }

  private:
    std::vector<char_t> buff;
    std::vector<node<char_t>> nodes;
  };

} // namespace clon

#endif