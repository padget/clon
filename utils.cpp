#include "utils.hpp"
#include <algorithm>
#include <stdexcept>

bool clon::utils::between(
  const std::size_t min,
  const std::size_t c,
  const std::size_t max)
{
  return min <= c and c <= max;
}

bool clon::utils::is_digit(const char c)
{
  return clon::utils::between('0', c, '9');
}

bool clon::utils::is_integer(std::string_view s)
{
  return !s.empty() and
    std::all_of(s.begin(), s.end(), clon::utils::is_digit);
}

bool clon::utils::is_lower(const char c)
{
  return clon::utils::between('a', c, 'z');
}

bool clon::utils::is_name(std::string_view s)
{
  return not s.empty() and std::all_of(s.begin(), s.end(), is_lower);
}

std::size_t clon::utils::to_integer(std::string_view v)
{
  std::size_t size = v.size();

  switch (size)
  {
    // TODO throw a meaning exception
  case 0: throw std::exception();
  case 1:
    return (v.back() - '0');
  default:
    std::string_view  v{ v.begin(), std::prev(v.end()) };
    std::size_t ten = 10 * clon::utils::to_integer(v);
    return (v.back() - '0') + ten;
  }
}


std::size_t clon::utils::count(const clon::utils::tokenizer& t)
{
  return std::count(t.b, t.e, t.sep);
}


bool clon::utils::operator==(
  const clon::utils::tokenizer& t1,
  const clon::utils::tokenizer& t2)
{
  return t1.b == t2.b and t1.e == t2.e and t1.sep == t2.sep;
}

clon::utils::tokenizer
clon::utils::begin(const clon::utils::tokenizer& t)
{
  return t;
}

clon::utils::tokenizer
clon::utils::end(const clon::utils::tokenizer& t)
{
  return { t.e, t.e, t.sep };
}

clon::utils::tokenizer&
clon::utils::operator++(clon::utils::tokenizer& t)
{
  while (t.b != t.e and *t.b != t.sep)
    ++t.b;

  if (t.b != t.e)
    ++t.b;

  return t;
}

clon::utils::tokenizer operator++(clon::utils::tokenizer& t, int)
{
  clon::utils::tokenizer tmp = t;
  operator++(t);
  return tmp;
}

std::string_view clon::utils::operator*(const clon::utils::tokenizer& t)
{
  auto tb = t.b;

  while (tb != t.e and *tb != t.sep)
    ++tb;

  if (tb != t.e)
    ++tb;

  return std::string_view{ t.b, tb };
}
