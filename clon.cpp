#include "clon.hpp"
#include "utils.hpp"
#include <tuple>


const clon::node& clon::undefined()
{
  return clon::basic::undefined();
}

clon::clon_type clon::type(const clon::node& c)
{
  return clon::basic::type(c);
}

const clon::node& clon::get(std::string_view path, const clon::node& c)
{
  return clon::path::get(path, c);
}

std::vector<std::reference_wrapper<const clon::node>> clon::get_all(
  std::string_view path, const clon::node& c)
{
  return clon::path::get_all(path, c);
}

const bool clon::check(std::string_view pth, std::string_view cstr, const clon::node& root)
{
  return clon::path::check(pth, cstr, root);
}

clon::root clon::parse(std::string_view s)
{
  return clon::parsing::parse(s);
}

clon::root clon::parse_fmt(std::string_view pattern, auto&&... as)
{
  return clon::parsing::parse_fmt(pattern, as...);
}

std::string clon::to_string(const clon::node& c)
{
  return clon::out::to_string(c);
}

std::string_view clon::to_original_string(const clon::root& c)
{
  return clon::out::to_original_string(c);
}
