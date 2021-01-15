#include "clon.hpp"
#include <tuple>

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
      // TODO throw a meaning exception
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

clon::model::node& clon::basic::undefined()
{
  static clon::model::node c;
  return c;
}

clon::model::clon_type clon::basic::type(const clon::model::node& c)
{
  switch (c.val.index())
  {
  case 0:
    return clon::model::clon_type::none;
  case 1:
    return clon::model::clon_type::boolean;
  case 2:
    return clon::model::clon_type::number;
  case 3:
    return clon::model::clon_type::string;
  case 4:
    return clon::model::clon_type::object;
  default:
    return clon::model::clon_type::none;
  }
}

namespace clon::path::model
{
  struct interval
  {
    std::size_t min;
    std::size_t max;
  };

  struct path
  {
    std::sv p;
    interval mnmx;
  };

  using paths = std::vector<path>;

  struct constraint
  {
    clon::model::clon_type type;
    interval mnmx;
  };
}

namespace clon::path
{
  clon::path::model::path to_path(std::sv spath);

  void explore(
    auto b, auto e,
    const clon::model::node& c,
    const auto& clb);

  void explore(
    std::sv pth,
    const clon::model::node& c,
    const auto& clb);


  std::size_t to_bound(std::sv b);

  clon::model::clon_type to_constraint_type(std::sv type);
  clon::path::model::interval to_constraint_interval(std::sv mnmx);
  clon::path::model::constraint to_constraint(std::sv cstr);
}

clon::path::malformed_path::malformed_path(std::sv reason)
  : std::invalid_argument(
    fmt::format("malformed path : {}", reason))
{}

clon::path::malformed_number::malformed_number(std::sv path)
  : malformed_path(
    fmt::format("malformed number '{}'", path))
{}

clon::path::malformed_name::malformed_name(std::sv path)
  : malformed_path(
    fmt::format("malformed name '{}'", path))
{}

clon::path::unreachable_path::unreachable_path(std::sv pth)
  : std::invalid_argument(
    fmt::format("unreachable path : {}", pth))
{}

clon::path::malformed_constraint::malformed_constraint(std::sv reason)
  : std::invalid_argument(
    fmt::format("malformed constraint : {}", reason))
{}


clon::path::model::path clon::path::to_path(std::sv spath)
{
  auto spl = spath | std::views::split(':');
  auto cnt = std::ranges::distance(spl);

  if (cnt > 2)
    throw clon::path::malformed_path(
      fmt::format("more than once ':' in path '{}'", spath));

  if (cnt == 0)
  {
    if (not utils::is_name(spath))
      throw clon::path::malformed_name(spath);

    return { spath, 0 };
  }
  else
  {
    std::array<std::sv, 2> pthnb;
    std::ranges::copy(
      spl | std::views::transform([](auto&& v) {
        return std::sv(
          &*std::ranges::begin(v),
          std::ranges::distance(v));}),
      std::begin(pthnb));

    auto&& pth = pthnb[0];

    if (pth.size() == 0)
      throw clon::path::malformed_path("empty path");

    if (not utils::is_name(pth))
      throw clon::path::malformed_name(pth);

    auto&& nb = pthnb[1];
    std::size_t min = 0;
    std::size_t max = 0;

    if (nb.size() != 0)
    {
      if (clon::utils::is_integer(nb))
      {
        min = clon::utils::to_integer(nb);
        max = min;
      }
      else if (nb == "*")
      {
        min = 0;
        max = std::numeric_limits<std::size_t>::max();
      }
      else
        throw clon::path::malformed_number(nb);
    }

    return { pth, {min , max} };
  }
}

void clon::path::explore(
  auto b, auto e,
  const clon::model::node& c,
  const auto& clb)
{
  if (b == e)
    clb(c);
  else if (clon::basic::is_<clon::model::object>(c))
  {
    const clon::path::model::path& pth = *b;
    std::size_t cnt(0);

    for (auto&& item : clon::basic::as_<clon::model::object>(c))
      if (item.name == pth.p)
      {
        if (clon::utils::between(pth.mnmx.min, cnt, pth.mnmx.max))
          clon::path::explore(std::next(b), e, item, clb);
        else if (cnt > pth.mnmx.max)
          break;

        cnt++;
      }
  }
}

void clon::path::explore(
  std::sv pth,
  const clon::model::node& c,
  const auto& clb)
{
  constexpr auto to_pth = [](auto&& v) {
    std::sv s(&*std::ranges::begin(v), std::ranges::distance(v));
    return clon::path::to_path(s);
  };

  auto&& pths = pth
    | std::views::split('.')
    | std::views::transform(to_pth);

  auto b = std::ranges::begin(pths);
  auto e = std::ranges::end(pths);

  explore(b, e, c, clb);
}

auto assign_to(const clon::model::node*& ref)
{
  return [&ref](const clon::model::node& c) {
    ref = &c;
  };
}

const clon::model::node& clon::path::get(
  std::sv pth,
  const clon::model::node& c)
{
  const clon::model::node* res = nullptr;
  explore(pth, c, assign_to(res));

  if (res == nullptr)
    return clon::basic::undefined();
  else
    return *res;
}

auto push_back(std::vector<std::reference_wrapper<const clon::model::node>>& refs)
{
  return [&](const clon::model::node& c)
  {
    refs.push_back(c);
  };
}

const std::vector<std::reference_wrapper<const clon::model::node>> clon::path::get_all(
  std::sv pth, const clon::model::node& c)
{
  std::vector<std::reference_wrapper<const clon::model::node>> refs;
  clon::path::explore(pth, c, push_back(refs));
  return refs;
}

std::size_t clon::path::to_bound(std::sv b)
{
  if (clon::utils::is_integer(b))
    return clon::utils::to_integer(b);
  else if (b == "*")
    return std::numeric_limits<std::size_t>::max();
  else
    throw clon::path::malformed_constraint(
      "the interval boundary must be a integer or '*'");
}

clon::model::clon_type clon::path::to_constraint_type(std::sv type)
{
  if (type.size() != 1)
    throw clon::path::malformed_constraint(
      "the type must be made of one letter :"
      " 's', 'o', 'b', 'n'");

  const char& c = *(type.begin());

  switch (c)
  {
  case 'o': return clon::model::clon_type::object;
  case 's': return clon::model::clon_type::string;
  case 'n': return clon::model::clon_type::number;
  case 'b': return clon::model::clon_type::boolean;
  default: throw clon::path::malformed_constraint(
    "the type must be made of one letter :"
    " 's', 'o', 'b', 'n'");
  }
}

clon::path::model::interval clon::path::to_constraint_interval(std::sv mnmx)
{
  auto b = mnmx.begin();
  auto e = mnmx.end();
  auto toks = clon::utils::tokenizer(b, e, '-');
  auto cnt = clon::utils::count(toks);

  if (cnt != 1)
    throw clon::path::malformed_constraint(
      "the interval must contains one '-'");

  std::size_t min = clon::path::to_bound(clon::utils::next_token(toks));
  std::size_t max = clon::path::to_bound(clon::utils::next_token(toks));

  if (min > max)
    throw clon::path::malformed_constraint(
      "the interval min must be inferior to max");

  return { min, max };
}

clon::path::model::constraint clon::path::to_constraint(std::sv cstr)
{
  auto b = cstr.begin();
  auto e = cstr.end();
  clon::utils::tokenizer toks(b, e, ':');
  std::size_t cnt = clon::utils::count(toks);

  if (cnt != 1)
    throw clon::path::malformed_constraint(
      "constraint must contain one ':'");

  return {
    clon::path::to_constraint_type(clon::utils::next_token(toks)),
    clon::path::to_constraint_interval(clon::utils::next_token(toks)) };
}

const bool clon::path::check(
  std::sv pth,
  std::sv cstr,
  const clon::model::node& root)
{
  const auto& all = clon::path::get_all(pth, root);
  const clon::path::model::constraint& cs = clon::path::to_constraint(cstr);

  if (not clon::utils::between(cs.mnmx.min, all.size(), cs.mnmx.max))
    return false;

  for (auto&& item : all)
    if (type(item.get()) != cs.type)
      return false;

  return true;
}

namespace clon::parsing
{
  template<typename char_iterator>
  clon::model::clon_type next_could_be(
    char_iterator b, char_iterator e);

  template<typename char_iterator>
  std::sv object_name(
    char_iterator b, char_iterator e);

  template<typename char_iterator>
  char_iterator parse_until(
    char_iterator b, char_iterator e, const char c);

  template<typename char_iterator>
  char_iterator parse_blank(
    char_iterator b, char_iterator e);

  template<typename char_iterator>
  std::tuple<clon::model::string, char_iterator> parse_string(
    char_iterator b, char_iterator e);

  template<typename char_iterator>
  std::tuple<clon::model::number, char_iterator> parse_number(
    char_iterator b, char_iterator e);

  template<typename char_iterator>
  std::tuple<clon::model::boolean, char_iterator> parse_boolean(
    char_iterator b, char_iterator e);

  template<typename char_iterator>
  std::tuple<clon::model::object, char_iterator> parse_object(
    char_iterator b, char_iterator e);

  template<typename char_iterator>
  std::tuple<clon::model::node, char_iterator> parse_one(
    char_iterator b, char_iterator e);

  clon::model::node parse(
    std::buffer::const_iterator b,
    std::buffer::const_iterator e);
}

clon::parsing::expected_character::expected_character(std::sv chars)
  : std::invalid_argument(
    fmt::format("expected characters : {}", chars))
{}

template<typename char_iterator>
clon::model::clon_type clon::parsing::next_could_be(
  char_iterator b, char_iterator e)
{
  if (b != e)
  {
    char c = *b;

    if (c == '"')
      return clon::model::clon_type::string;
    else if (c == '(')
      return clon::model::clon_type::object;
    else if (c == 't')
      return clon::model::clon_type::boolean;
    else if (c == 'f')
      return clon::model::clon_type::boolean;
    else if (utils::between('0', c, '9'))
      return clon::model::clon_type::number;
  }

  return clon::model::clon_type::none;
}

template<typename char_iterator>
std::sv clon::parsing::object_name(
  char_iterator b, char_iterator e)
{
  auto sb = b;

  while (clon::utils::between('a', *b, 'z'))
    std::advance(b, 1);

  return std::sv(sb, b);
}

template<typename char_iterator>
char_iterator clon::parsing::parse_until(
  char_iterator b, char_iterator e, const char c)
{
  while (b != e and *b != c)
    std::advance(b, 1);

  if (b == e)
    throw clon::parsing::expected_character(
      fmt::format("'{}'", c));

  return b;
}

template<typename char_iterator>
char_iterator clon::parsing::parse_blank(
  char_iterator b, char_iterator e)
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
std::tuple<clon::model::string, char_iterator> clon::parsing::parse_string(
  char_iterator b, char_iterator e)
{
  if (b != e and *b == '"')
  {
    auto sb = b;
    b = clon::parsing::parse_until(std::next(b), e, '"');

    if (b != e and *b == '"')
      return {
        clon::model::string(std::next(sb), b),
        std::next(b) };
    else
      throw clon::parsing::expected_character("'\"'");
  }

  throw clon::parsing::expected_character("'\"'");
}

template<typename char_iterator>
std::tuple<clon::model::number, char_iterator> clon::parsing::parse_number(
  char_iterator b, char_iterator e)
{
  if (b != e and clon::utils::between('0', *b, '9'))
  {
    auto sb = b;

    while (b != e and clon::utils::between('0', *b, '9'))
      std::advance(b, 1);

    if (b != e and *b == '.')
    {
      std::advance(b, 1);

      while (b != e and clon::utils::between('0', *b, '9'))
        std::advance(b, 1);
    }

    return { std::stod(std::string(sb, b)), b };
  }

  throw clon::parsing::expected_character("[0-9]");
}

template<typename char_iterator>
std::tuple<clon::model::boolean, char_iterator> clon::parsing::parse_boolean(
  char_iterator b, char_iterator e)
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

  throw clon::parsing::expected_character("true|false");
}

template<typename char_iterator>
std::tuple<clon::model::object, char_iterator> clon::parsing::parse_object(
  char_iterator b,
  char_iterator e)
{
  if (b == e or *b != '(')
    throw clon::parsing::expected_character("'('");

  object cls;

  while (b != e and *b == '(')
  {
    auto&& res = parse_one(b, e);
    cls.push_back(std::get<0>(res));
    b = std::get<1>(res);
    b = parse_blank(b, e);
  }

  return { cls, b };
}

template<typename char_iterator>
std::tuple<clon::model::node, char_iterator> clon::parsing::parse_one(
  char_iterator b, char_iterator e)
{
  if (b == e or *b != '(')
    throw clon::parsing::expected_character("'('");

  clon::model::node c;
  b = clon::parsing::parse_blank(std::next(b), e);
  c.name = clon::parsing::object_name(b, e);
  std::advance(b, c.name.size());
  b = clon::parsing::parse_blank(b, e);

  switch (clon::parsing::next_could_be(b, e))
  {
  case clon::model::clon_type::boolean:
  {
    auto&& bl = clon::parsing::parse_boolean(b, e);
    b = std::get<1>(bl);
    c.val = std::get<0>(bl);
    break;
  }
  case clon::model::clon_type::number:
  {
    auto&& nb = clon::parsing::parse_number(b, e);
    b = std::get<1>(nb);
    c.val = std::get<0>(nb);
    break;
  }
  case clon::model::clon_type::string:
  {
    auto&& st = clon::parsing::parse_string(b, e);
    b = std::get<1>(st);
    c.val = std::get<0>(st);
    break;
  }
  case clon::model::clon_type::object:
  {
    auto&& ob = clon::parsing::parse_object(b, e);
    b = std::get<1>(ob);
    c.val = std::get<0>(ob);
    break;
  }
  case clon::model::clon_type::none:
    throw clon::parsing::expected_character("(|true|false|'\"'|[0-9]");
  }

  b = clon::parsing::parse_blank(b, e);

  if (b != e and *b == ')')
    return { std::move(c), std::next(b) };

  throw clon::parsing::expected_character(")");
}

clon::model::node clon::parsing::parse(
  std::buffer::const_iterator b,
  std::buffer::const_iterator e)
{
  b = clon::parsing::parse_blank(b, e);
  auto&& res = clon::parsing::parse_one(b, e);
  b = std::get<1>(res);
  b = clon::parsing::parse_blank(b, e);

  if (b == e)
    return std::get<0>(res);

  throw clon::parsing::expected_character("EOF|'\\0'");
}

clon::model::root clon::parsing::parse(std::sv s)
{
  clon::model::root root;
  root.buff = { s.begin(), s.end() };
  root.root = parse(root.buff.begin(), root.buff.end());
  return root;
}


namespace clon::out
{
  void to_string_basic(
    fmt::memory_buffer& buf,
    const clon::model::node& c);

  void to_string_boolean(
    fmt::memory_buffer& buf,
    const clon::model::node& c);

  void to_string_string(
    fmt::memory_buffer& buf,
    const clon::model::node& c);

  void to_string_number(
    fmt::memory_buffer& buf,
    const clon::model::node& c);

  void to_string_object(
    fmt::memory_buffer& buf,
    const clon::model::node& c);
}

void clon::out::to_string_basic(
  fmt::memory_buffer& buf,
  const clon::model::node& c);

void clon::out::to_string_boolean(
  fmt::memory_buffer& buf,
  const clon::model::node& c)
{
  fmt::format_to(buf, "{}", (clon::basic::as_<clon::model::boolean>(c) ? "true" : "false"));
}

void clon::out::to_string_string(
  fmt::memory_buffer& buf,
  const clon::model::node& c)
{
  fmt::format_to(buf, "\"{}\"", clon::basic::as_<clon::model::string>(c));
}

void clon::out::to_string_number(
  fmt::memory_buffer& buf,
  const clon::model::node& c)
{
  fmt::format_to(buf, "{}", clon::basic::as_<clon::model::number>(c));
}

void clon::out::to_string_object(
  fmt::memory_buffer& buf,
  const clon::model::node& c)
{
  for (auto&& item : clon::basic::as_<clon::model::object>(c))
    clon::out::to_string_basic(buf, item);
}

void clon::out::to_string_basic(
  fmt::memory_buffer& buf,
  const clon::model::node& c)
{
  fmt::format_to(buf, "({} ", c.name);

  switch (type(c))
  {
  case clon::model::clon_type::boolean:
    clon::out::to_string_boolean(buf, c);
    break;
  case clon::model::clon_type::number:
    clon::out::to_string_number(buf, c);
    break;
  case clon::model::clon_type::string:
    clon::out::to_string_string(buf, c);
    break;
  case clon::model::clon_type::object:
    clon::out::to_string_object(buf, c);
    break;
  case clon::model::clon_type::none:
    break;
  }

  fmt::format_to(buf, ")");
}

std::string clon::out::to_string(const clon::model::node& c)
{
  fmt::memory_buffer buf;
  clon::out::to_string_basic(buf, c);
  return fmt::to_string(buf);
}

std::sv clon::out::to_original_string(const root& c)
{
  return { c.buff.begin(), c.buff.end() };
}

void clon::in::to_clon(
  clon::in::model::temporary_buffer& tbuf,
  std::sv name, const bool& b)
{
  constexpr std::sv trues = "true";
  constexpr std::sv falses = "false";
  std::sv bs = b ? trues : falses;
  fmt::format_to(tbuf, R"(({} {}))", name, bs);
}

clon::node& clon::undefined()
{
  return clon::basic::undefined();
}

clon::clon_type clon::type(const clon::node& c)
{
  return clon::basic::type(c);
}

const clon::node& clon::get(std::sv path, const clon::node& c)
{
  return clon::path::get(path, c);
}

std::vector<std::reference_wrapper<const clon::node>> clon::get_all(
  std::sv path, const clon::node& c)
{
  return clon::path::get_all(path, c);
}

const bool clon::check(std::sv pth, std::sv cstr, const clon::node& root)
{
  return clon::path::check(pth, cstr, root);
}

clon::root clon::parse(std::sv s)
{
  return clon::parsing::parse(s);
}

clon::root clon::parse_fmt(std::sv pattern, auto&&... as)
{
  return clon::parsing::parse_fmt(pattern, as...);
}

std::string clon::to_string(const clon::node& c)
{
  return clon::out::to_string(c);
}

std::sv clon::to_original_string(const clon::root& c)
{
  return clon::out::to_original_string(c);
}
