#include "parsing.hpp"
#include "model.hpp"
#include "utils.hpp"

#include <stdexcept>
#include <fmt/format.h>
#include <vector>

namespace clon::parsing
{
  template<typename char_iterator>
  clon::model::clon_type next_could_be(
    char_iterator b, char_iterator e);

  template<typename char_iterator>
  std::string_view object_name(
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
    std::vector<char>::const_iterator b,
    std::vector<char>::const_iterator e);
}

clon::parsing::expected_character::expected_character(std::string_view chars)
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
    else if (clon::utils::between('0', c, '9'))
      return clon::model::clon_type::number;
  }

  return clon::model::clon_type::none;
}

template<typename char_iterator>
std::string_view clon::parsing::object_name(
  char_iterator b, char_iterator e)
{
  auto sb = b;

  while (clon::utils::between('a', *b, 'z'))
    std::advance(b, 1);

  return std::string_view(sb, b);
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

  clon::model::object  cls;

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
  std::vector<char>::const_iterator b,
  std::vector<char>::const_iterator e)
{
  b = clon::parsing::parse_blank(b, e);
  auto&& res = clon::parsing::parse_one(b, e);
  b = std::get<1>(res);
  b = clon::parsing::parse_blank(b, e);

  if (b == e)
    return std::get<0>(res);

  throw clon::parsing::expected_character("EOF|'\\0'");
}

clon::model::root clon::parsing::parse(std::string_view s)
{
  clon::model::root root;
  root.buff = { s.begin(), s.end() };
  root.root = parse(root.buff.begin(), root.buff.end());
  return root;
}

