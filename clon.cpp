#include "clon.hpp"

namespace clon::utils
{
  bool between(
    const char min,
    const char c,
    const char max)
  {
    return min <= c and c <= max;
  }
  
  bool is_digit(const char c)
  {
    return '0' <= c and c <= '9';
  }

  bool is_integer(std::sv s)
  {
    return !s.empty() and
      std::all_of(s.begin(), s.end(), is_digit);
  }

  bool is_lower(const char c)
  {
    return between('a', c, 'z');
  }

  bool is_name(std::sv s)
  {
    return !s.empty() and
      std::all_of(s.begin(), s.end(), is_lower);
  }
  
  std::size_t to_integer(std::sv v)
  {
    std::size_t size = v.size();

    switch (size)
    {
    case 0: throw std::exception();
    case 1:
      return (v.back() - '0');
    default:
      std::sv v{ v.begin(), std::prev(v.end()) };
      std::size_t ten = 10 * to_integer(v);
      return (v.back() - '0') + ten;
    }
  }
}

namespace clon::parser::detail
{
  template<typename char_iterator>
  clon_type next_could_be(
    char_iterator b,
    char_iterator e)
  {
    if (b != e)
    {
      if (*b == '"')
        return clon_type::string;
      else if (*b == '(')
        return clon_type::object;
      else if (*b == 't')
        return clon_type::boolean;
      else if (*b == 'f')
        return clon_type::boolean;
      else if ('0' <= *b and *b <= '9')
        return clon_type::number;
    }

    return clon_type::none;
  }



  template<typename char_iterator>
  std::string object_name(
    char_iterator b,
    char_iterator e)
  {
    auto sb = b;

    while (utils::between('a', *b, 'z'))
      std::advance(b, 1);

    return std::string(sb, b);
  }

  template<typename type_t, typename iterator>
  struct parse_result
  {
    type_t val;
    iterator i;
  };


  template<typename char_iterator>
  char_iterator parse_until(
    char_iterator b,
    char_iterator e,
    const char c)
  {
    while (b != e and *b != c)
      std::advance(b, 1);

    if (b == e)
      throw expected_character(
        fmt::format("'{}'", c));

    return b;
  }

  template<typename char_iterator>
  char_iterator parse_blank(
    char_iterator b,
    char_iterator e)
  {
    while (b != e and (*b == ' ' or
      *b == '\t' or
      *b == '\n' or
      *b == '\r'))
      std::advance(b, 1);
    return b;
  }

  template<typename char_iterator>
  parse_result<string, char_iterator> parse_string(
    char_iterator b,
    char_iterator e)
  {
    if (b != e and *b == '"')
    {
      auto sb = b;
      b = parse_until(std::next(b), e, '"');

      if (b != e and *b == '"')
        return {
          std::string(std::next(sb), b),
          std::next(b) };
      else
        throw expected_character("'\"'");
    }

    throw expected_character("'\"'");
  }

  template<typename char_iterator>
  parse_result<number, char_iterator> parse_number(
    char_iterator b,
    char_iterator e)
  {
    if (b != e and utils::between('0', *b, '9'))
    {
      auto sb = b;

      while (b != e and utils::between('0', *b, '9'))
        std::advance(b, 1);

      return { std::stod(std::string(sb, b)), b };
    }

    throw expected_character("[0-9]");
  }

  template<typename char_iterator>
  parse_result<boolean, char_iterator> parse_bool(
    char_iterator b,
    char_iterator e)
  {
    auto b0 = b;
    auto b1 = b0 != e ? std::next(b0) : e;
    auto b2 = b1 != e ? std::next(b1) : e;
    auto b3 = b2 != e ? std::next(b2) : e;
    auto b4 = b3 != e ? std::next(b3) : e;
    auto b5 = b4 != e ? std::next(b4) : e;

    if (b0 != e and *b0 == 't' and
      b1 != e and *b1 == 'r' and
      b2 != e and *b2 == 'u' and
      b3 != e and *b3 == 'e')
      return { true, b4 };
    else if (b0 != e and *b0 == 'f' and
      b1 != e and *b1 == 'a' and
      b2 != e and *b2 == 'l' and
      b3 != e and *b3 == 's' and
      b4 != e and *b4 == 'e')
      return { false, b5 };

    throw expected_character("true|false");
  }

  template<typename char_iterator>
  parse_result<clon, char_iterator> parse_one(
    char_iterator b,
    char_iterator e);

  template<typename char_iterator>
  parse_result<object, char_iterator> parse_object(
    char_iterator b,
    char_iterator e)
  {
    if (b == e or *b != '(')
      throw expected_character("'('");

    object cls;

    while (b != e and *b == '(')
    {
      auto&& res = parse_one(b, e);
      cls.push_back(res.val);
      b = res.i;
      b = parse_blank(b, e);
    }

    return { cls, b };
  }

  template<typename char_iterator>
  parse_result<clon, char_iterator> parse_one(
    char_iterator b,
    char_iterator e)
  {
    if (b == e or *b != '(')
      throw expected_character("'('");

    clon c;
    b = parse_blank(std::next(b), e);
    c.name = object_name(b, e);
    std::advance(b, c.name.size());
    b = parse_blank(b, e);

    switch (next_could_be(b, e))
    {
    case clon_type::boolean:
    {
      auto&& bl = parse_bool(b, e);
      b = bl.i;
      c.val = bl.val;
      break;
    }
    case clon_type::number:
    {
      auto&& nb = parse_number(b, e);
      b = nb.i;
      c.val = nb.val;
      break;
    }
    case clon_type::string:
    {
      auto&& st = parse_string(b, e);
      b = st.i;
      c.val = st.val;
      break;
    }
    case clon_type::object:
    {
      auto&& ob = parse_object(b, e);
      b = ob.i;
      c.val = ob.val;
      break;
    }
    case clon_type::none:
      throw expected_character("(|true|false|'\"'|[0-9]");
    }

    b = parse_blank(b, e);

    if (b != e and *b == ')')
      return { std::move(c), std::next(b) };

    throw expected_character(")");
  }

  template<typename char_iterator>
  clon parse(
    char_iterator b,
    char_iterator e)
  {
    b = detail::parse_blank(b, e);
    auto&& res = detail::parse_one(b, e);
    b = res.i;
    b = detail::parse_blank(b, e);

    if (res.i == e)
      return res.val;

    // TODO error a définir
    throw std::exception();
  }
} // namespace detail

namespace clon::getter::detail
{
  struct path
  {
    std::sv p;
    std::size_t idx = 0;
    std::boolean many = false;
  };

  using paths = std::vector<path>;

  struct tokenizer
  {
    std::sv::const_iterator b;
    std::sv::const_iterator e;
    const char sep;
  };

  std::size_t count(const tokenizer& t)
  {
    return std::count(t.b, t.e, t.sep);
  }

  std::sv next_token(tokenizer& t)
  {
    auto b = t.b;

    while (t.b != t.e)
      if (*(t.b) == t.sep)
      {
        std::advance(t.b, 1);
        break;
      }
      else
        std::advance(t.b, 1);

    if (t.b == t.e)
      return std::sv(b, t.b);
    else
      return std::sv(b, std::prev(t.b));
  }



  path to_path(std::sv spath)
  {
    auto toks = tokenizer(spath.begin(), spath.end(), ':');
    auto cnt = count(toks);

    if (cnt > 2)
      throw malformed_path(
        fmt::format("more than once ':' in path '{}'", spath));

    if (cnt == 0)
    {
      if (not utils::is_name(spath))
        throw malformed_name(spath);

      return { spath, 0 };
    }
    else
    {
      auto&& pth = next_token(toks);

      if (pth.size() == 0)
        throw malformed_path("empty path");

      if (not utils::is_name(pth))
        throw malformed_name(pth);

      auto&& nb = next_token(toks);
      std::size_t idx = 0;
      std::boolean many = false;

      if (nb.size() != 0)
      {
        if (utils::is_integer(nb))
          idx = utils::to_integer(nb);
        else if (nb == "*")
        {
          idx = std::numeric_limits<std::size_t>::max();
          many = true;
        }
        else
          throw malformed_number(nb);
      }

      return { pth, idx , many };
    }
  }

  paths to_paths(std::sv spath)
  {
    paths pths;
    tokenizer toks(spath.begin(), spath.end(), '.');
    std::sv tok;

    while ((tok = next_token(toks)).size() > 0)
      pths.push_back(to_path(tok));

    return pths;
  }

  const clon& get_mono(
    const path& pth,
    const clon& c)
  {
    if (is_object(c))
    {
      std::size_t cnt = 0;

      for (const clon& sub : as_object(c))
        if (sub.name == pth.p)
        {
          if (cnt == pth.idx)
            return sub;
          else
            cnt++;
        }
    }

    return undefined();
  }

  const clon& get_mono(
    paths::const_iterator b,
    paths::const_iterator e,
    const clon& c)
  {
    if (b != e and not is_none(c))
    {
      const clon& sub = get_mono(*b, c);
      std::advance(b, 1);
      return get_mono(b, e, sub);
    }
    else if (b == e and is_none(c))
      return undefined();
    else if (b == e and not is_none(c))
      return c;
    else
      return undefined();
  }

  using clon_refs = std::vector<std::reference_wrapper<const clon>>;


}




namespace clon
{
  clon& undefined()
  {
    static clon c;
    return c;
  }

  clon_type type(const clon& c)
  {
    switch (c.val.index())
    {
    case 0:
      return clon_type::none;
    case 1:
      return clon_type::boolean;
    case 2:
      return clon_type::number;
    case 3:
      return clon_type::string;
    case 4:
      return clon_type::object;
    default:
      return clon_type::none;
    }
  }

  bool is_none(const clon& c)
  {
    return type(c) == clon_type::none;
  }

  bool is_bool(const clon& c)
  {
    return type(c) == clon_type::boolean;
  }

  bool is_number(const clon& c)
  {
    return type(c) == clon_type::number;
  }

  bool is_string(const clon& c)
  {
    return type(c) == clon_type::string;
  }

  bool is_object(const clon& c)
  {
    return type(c) == clon_type::object;
  }

  bool is_none(std::sv pth, const clon& c)
  {
    return is_none(get(pth, c));
  }

  bool is_bool(std::sv pth, const clon& c)
  {
    return is_bool(get(pth, c));
  }

  bool is_number(std::sv pth, const clon& c)
  {
    return is_number(get(pth, c));
  }

  bool is_string(std::sv pth, const clon& c)
  {
    return is_string(get(pth, c));
  }

  bool is_object(std::sv pth, const clon& c)
  {
    return is_object(get(pth, c));
  }

  boolean_cref as_bool(const clon& c)
  {
    return std::get<boolean>(c.val);
  }

  string_cref as_string(const clon& c)
  {
    return std::get<string>(c.val);
  }

  number_cref as_number(const clon& c)
  {
    return std::get<number>(c.val);
  }

  object_cref as_object(const clon& c)
  {
    return std::get<object>(c.val);
  }

  expected_character::expected_character(std::sv chars)
    : std::invalid_argument(
      fmt::format("expected characters : {}", chars))
  {}

  malformed_path::malformed_path(std::sv reason)
    : std::invalid_argument(
      fmt::format("malformed path : {}", reason))
  {}

  malformed_number::malformed_number(std::sv path)
    : malformed_path(
      fmt::format("malformed number '{}'", path))
  {}

  malformed_name::malformed_name(std::sv path)
    : malformed_path(
      fmt::format("malformed name '{}'", path))
  {}

  unreachable_path::unreachable_path(std::sv pth)
    : std::invalid_argument(
      fmt::format("unreachable path : {}", pth))
  {}

  clon parse(std::sv s)
  {
    return parser::detail::parse(s.begin(), s.end());
  }

  clon parse_stream(std::istream& s)
  {
    std::istreambuf_iterator<char> in(s);
    std::istreambuf_iterator<char> eos;
    return parser::detail::parse(in, eos);
  }

  const clon& get(std::sv path, const clon& c)
  {
    getter::detail::paths&& pths = getter::detail::to_paths(path);
    return getter::detail::get_mono(pths.begin(), pths.end(), c);
  }

  const string& get_string(std::sv pth, const clon& c)
  {
    return as_string(get(pth, c));
  }

  const number& get_number(std::sv pth, const clon& c)
  {
    return as_number(get(pth, c));
  }

  const boolean& get_boolean(std::sv pth, const clon& c)
  {
    return as_bool(get(pth, c));
  }

  const object& get_object(std::sv pth, const clon& c)
  {
    return as_object(get(pth, c));
  }

  const bool exists(std::sv pth, const clon& c)
  {
    return is_none(get(pth, c));;
  }

  const bool check(std::sv pth, std::sv cstr, const clon& root)
  {
    return true;
  }
}

