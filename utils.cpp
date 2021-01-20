#include "utils.hpp"
#include <algorithm>
#include <exception>
#include <stdexcept>

bool clon::utils::between(
  const char min,
  const char c,
  const char max)
{
  return min <= c and c <= max;
}

bool clon::utils::is_digit(const char c)
{
  return '0' <= c and c <= '9';
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

std::string_view clon::utils::next_token(clon::utils::tokenizer& t)
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
    return std::string_view(b, t.b);
  else
    return std::string_view(b, std::prev(t.b));
}
