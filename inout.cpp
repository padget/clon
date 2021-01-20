#include "inout.hpp"
#include <fmt/format.h>
#include "model.hpp"
#include "utils.hpp"
#include "parsing.hpp"

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

  switch (clon::basic::type(c))
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

std::string_view clon::out::to_original_string(const clon::model::root& c)
{
  return { c.buff.begin(), c.buff.end() };
}

void clon::in::to_clon(
  clon::in::model::temporary_buffer& tbuf,
  std::string_view name, const bool& b)
{
  constexpr std::string_view trues = "true";
  constexpr std::string_view falses = "false";
  std::string_view bs = b ? trues : falses;
  fmt::format_to(tbuf, R"(({} {}))", name, bs);
}
