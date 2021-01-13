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

  bool is_name(auto&& s)
  {
    return
      not std::ranges::empty(s) and
      std::ranges::all_of(s, is_lower);
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
      char c = *b;

      if (c == '"')
        return clon_type::string;
      else if (c == '(')
        return clon_type::object;
      else if (c == 't')
        return clon_type::boolean;
      else if (c == 'f')
        return clon_type::boolean;
      else if (utils::between('0', c, '9'))
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
    while (b != e and
      (*b == ' ' or
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

    if (b == e)
      return res.val;

    // TODO error a définir
    throw expected_character("EOF|'\\0'");;
  }
} // namespace detail

namespace clon::getter::detail
{
  struct path
  {
    std::sv p;
    std::size_t min = 0;
    std::size_t max = 0;
  };

  using paths = std::vector<path>;

  template <typename view_t>
  std::sv to_sv(view_t&& v)
  {
    return std::sv(
      &*std::ranges::begin(v),
      std::ranges::distance(v));
  }

  path to_path(auto&& spath)
  {
    auto spl = spath | std::views::split(':');
    auto cnt = std::ranges::distance(spl);

    if (cnt > 2)
      throw malformed_path(
        fmt::format("more than once ':' in path '{}'", to_sv(spath)));

    if (cnt == 0)
    {
      if (not utils::is_name(spath))
        throw malformed_name(to_sv(spath));

      return { to_sv(spath), 0 };
    }
    else
    {
      std::array<std::sv, 2> pthnb;
      std::ranges::copy(
        spl | std::views::transform([](auto&& v) {return to_sv(v);}),
        std::begin(pthnb));

      auto&& pth = pthnb[0];

      if (pth.size() == 0)
        throw malformed_path("empty path");

      if (not utils::is_name(pth))
        throw malformed_name(pth);

      auto&& nb = pthnb[1];
      std::size_t min = 0;
      std::size_t max = 0;

      if (nb.size() != 0)
      {
        if (utils::is_integer(nb))
        {
          min = utils::to_integer(nb);
          max = min;
        }
        else if (nb == "*")
        {
          min = 0;
          max = std::numeric_limits<std::size_t>::max();
        }
        else
          throw malformed_number(nb);
      }

      return { pth, min , max };
    }
  }

  auto has_same_name(
    const path& pth)
  {
    return [&pth](auto&& c) {
      return c.name == pth.p;
    };
  }

  template<typename clbt>
  void explore(
    auto b,
    auto e,
    const clon& c,
    const clbt& clb)
  {
    if (b == e)
      clb(c);
    else if (is_object(c))
    {
      const path& pth = *b;
      std::size_t cnt(0);

      for (auto&& item : as_object(c))
        if (item.name == pth.p)
        {
          if (utils::between(pth.min, cnt, pth.max))
            explore(std::next(b), e, item, clb);
          else if (cnt > pth.max)
            break;

          cnt++;
        }
    }
  }

  template<typename clbt>
  void explore(
    std::sv pth,
    const clon& c,
    const clbt& clb)
  {
    constexpr auto to_pth = [](auto&& v) {
      return to_path(v);
    };

    auto&& pths = pth
      | std::views::split('.')
      | std::views::transform(to_pth);

    auto b = std::ranges::begin(pths);
    auto e = std::ranges::end(pths);

    explore(b, e, c, clb);
  }

  auto abufign_to(const clon*& ref)
  {
    return [&ref](const clon& c) {
      ref = &c;
    };
  }

  const clon& get(
    std::sv pth,
    const clon& c)
  {
    const clon* res = nullptr;
    explore(pth, c, abufign_to(res));

    if (res == nullptr)
      return undefined();
    else
      return *res;
  }

  auto push_back(clon_refs& refs)
  {
    return [&](const clon& c) {
      refs.push_back(c);
    };
  }

  clon_crefs get_all(
    std::sv pth,
    const clon& c)
  {
    clon_refs refs;
    explore(pth, c, push_back(refs));
    return refs;
  }
}

namespace clon::checker::detail
{
  struct interval
  {
    std::size_t min;
    std::size_t max;
  };

  struct constraint
  {
    clon_type type;
    interval mnmx;
  };

  struct constrained_path
  {
    std::sv path;
    std::size_t min;
    std::size_t max;
  };

  auto is_specific_char(const char spec)
  {
    return [=](const char& c)
    {
      return c == spec;
    };
  }

  bool contains(
    std::sv s,
    const char c)
  {
    auto b = s.begin();
    auto e = s.end();
    return std::any_of(
      b, e, is_specific_char(c));
  }


  bool must_be_string(
    std::sv constraint)
  {
    return contains(constraint, 's');
  }

  bool must_be_object(
    std::sv constraint)
  {
    return contains(constraint, 'o');
  }

  bool must_be_number(
    std::sv constraint)
  {
    return contains(constraint, 'n');
  }

  bool must_be_boolean(
    std::sv constraint)
  {
    return contains(constraint, 'b');
  }

  class malformed_constraint
    : public std::invalid_argument
  {
  public:
    malformed_constraint(
      std::sv reason)
      : std::invalid_argument(
        fmt::format(
          "malformed constraint : {}", reason))
    {}
  };

  clon_type to_constraint_type(
    std::sv type)
  {
    if (type.size() != 1)
      throw malformed_constraint(
        "the type must be made of one letter");

    const char& c = *(type.begin());

    switch (c)
    {
    case 'o': return clon_type::object;
    case 's': return clon_type::string;
    case 'n': return clon_type::number;
    case 'b': return clon_type::boolean;
    default: throw malformed_constraint(
      "the type must be made of one letter :"
      " 's', 'o', 'b', 'n'");
    }
  }

  std::size_t to_bound(
    std::sv b)
  {
    if (utils::is_integer(b))
      return utils::to_integer(b);
    else if (b == "*")
      return std::numeric_limits<std::size_t>::max();
    else
      throw malformed_constraint(
        "the interval boundary must be a integer or '*'");
  }

  interval to_constraint_interval(
    std::sv mnmx)
  {
    auto b = mnmx.begin();
    auto e = mnmx.end();
    auto toks = utils::tokenizer(b, e, '-');
    auto cnt = utils::count(toks);

    if (cnt != 1)
      throw malformed_constraint(
        "the interval must contains one '-'");

    auto&& min = to_bound(utils::next_token(toks));
    auto&& max = to_bound(utils::next_token(toks));

    if (min > max)
      throw malformed_constraint(
        "the interval min must be inferior to max");

    return { min, max };
  }

  constraint to_constraint(
    std::sv cstr)
  {
    auto b = cstr.begin();
    auto e = cstr.end();
    auto toks = utils::tokenizer(b, e, ':');
    auto cnt = utils::count(toks);

    if (cnt != 1)
      throw malformed_constraint(
        "constraint must contain one ':'");

    auto&& type = to_constraint_type(utils::next_token(toks));
    auto&& mnmx = to_constraint_interval(utils::next_token(toks));

    return { type, mnmx };
  }
}

namespace clon::stringify::detail
{
  inline void to_string_basic(
    fmt::memory_buffer& buf,
    const clon& c);

  inline void to_string_bool(
    fmt::memory_buffer& buf,
    const clon& c)
  {
    fmt::format_to(buf, "{}", (as_bool(c) ? "true" : "false"));
  }

  inline void to_string_string(
    fmt::memory_buffer& buf,
    const clon& c)
  {
    fmt::format_to(buf, "\"{}\"", as_string(c));
  }

  inline void to_string_number(
    fmt::memory_buffer& buf,
    const clon& c)
  {
    fmt::format_to(buf, "{}", as_number(c));
  }

  inline void to_string_object(
    fmt::memory_buffer& buf,
    const clon& c)
  {
    for (auto&& item : as_object(c))
      to_string_basic(buf, item);
  }

  inline void to_string_basic(
    fmt::memory_buffer& buf,
    const clon& c)
  {
    fmt::format_to(buf, "({} ", c.name);

    switch (type(c))
    {
    case clon_type::boolean:
      to_string_bool(buf, c);
      break;
    case clon_type::number:
      to_string_number(buf, c);
      break;
    case clon_type::string:
      to_string_string(buf, c);
      break;
    case clon_type::object:
      to_string_object(buf, c);
      break;
    case clon_type::none:
      break;
    }

    fmt::format_to(buf, ")");
  }

  std::string to_string(const clon& c)
  {
    fmt::memory_buffer buf;
    to_string_basic(buf, c);
    return fmt::to_string(buf);
  }
}

namespace clon
{
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
    return getter::detail::get(path, c);
  }

  clon_crefs get_all(std::sv path, const clon& c)
  {
    return getter::detail::get_all(path, c);
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

  const bool check(
    std::sv pth,
    std::sv cstr,
    const clon& root)
  {
    const auto& all = get_all(pth, root);
    const checker::detail::constraint& cs = checker::detail::to_constraint(cstr);

    std::size_t cnt = all.size();

    if (not utils::between(cs.mnmx.min, all.size(), cs.mnmx.max))
      return false;

    for (auto&& item : all)
      if (type(item.get()) != cs.type)
        return false;

    return true;
  }

  std::string to_string(const clon& c)
  {
    return stringify::detail::to_string(c);
  }

  template<typename type_t>
  const clon to_clon(const type_t& clon)
  {

  }
}

