#ifndef __clon_inout_hpp__
#define __clon_inout_hpp__

#include "model.hpp"
#include "parsing.hpp"
#include <string>
#include <string_view>
#include <fmt/format.h>

namespace clon::out
{
  std::string to_string(const clon::model::node& c);
  std::string_view to_original_string(const clon::model::root& c);
}

namespace clon::in::model
{
  template<typename type_t>
  struct in_pair
  {
    std::string_view name;
    const type_t& val;
  };

  template<typename iterator_t>
  struct in_sequence
  {
    std::string_view name;
    iterator_t b;
    iterator_t e;
  };

  using temporary_buffer = fmt::memory_buffer;
}

namespace clon::in::constraint::detail
{
  template<typename type_t>
  constexpr bool is_pair_v = false;

  template<typename type_t>
  constexpr bool is_pair_v<clon::in::model::in_pair<type_t>> = true;

  template<typename type_t>
  constexpr bool is_sequence_v = false;

  template<typename type_t>
  constexpr bool is_sequence_v<clon::in::model::in_sequence<type_t>> = true;
}

namespace clon::in::constraint
{
  template<typename type_t>
  concept is_pair = clon::in::constraint::detail::is_pair_v<type_t>;

  template<typename type_t>
  concept is_sequence = clon::in::constraint::detail::is_sequence_v<type_t>;

  template<typename type_t>
  concept is_convertible =
    is_pair<type_t> or
    is_sequence<type_t>;
}

namespace clon::in::make
{
  template <typename type_t>
  clon::in::model::in_pair<type_t> pair(
    std::string_view name, const type_t& val)
  {
    return { name, val };
  }

  template<typename iterator_t>
  clon::in::model::in_sequence<iterator_t> sequence(
    std::string_view name, iterator_t b, iterator_t e)
  {
    return  { name, b, e };
  }
}

namespace clon::in
{
  template<clon::constraint::number type_t>
  void to_clon(
    clon::in::model::temporary_buffer& tbuf,
    std::string_view name, const type_t& val)
  {
    fmt::format_to(tbuf, R"(({} {}))", name, val);
  }

  template<clon::constraint::char_sequence type_t>
  void to_clon(
    clon::in::model::temporary_buffer& tbuf,
    std::string_view name, const type_t& val)
  {
    fmt::format_to(tbuf, R"(({} "{}"))", name, val);
  }

  template<clon::in::constraint::is_pair p >
  void to_clon_choice(
    clon::in::model::temporary_buffer& tbuf,
    std::string_view name,
    const p& ar)
  {
    to_clon(tbuf, ar.name, ar.val);
  }

  template<clon::in::constraint::is_sequence seq>
  void to_clon_choice(
    clon::in::model::temporary_buffer& tbuf,
    std::string_view name,
    const seq& ar)
  {
    auto b = ar.b;
    auto e = ar.e;

    while (b != e)
    {
      to_clon(tbuf, ar.name, *b);
      std::advance(b, 1);
    }
  }

  template<clon::in::constraint::is_convertible ... args_t>
  void to_clon(
    clon::in::model::temporary_buffer& tbuf,
    std::string_view name,
    const args_t& ... arg)
  {
    fmt::format_to(tbuf, "({} ", name);
    (to_clon_choice(tbuf, arg.name, arg), ...);
    fmt::format_to(tbuf, ")");
  }

  void to_clon(
    clon::in::model::temporary_buffer& tbuf,
    std::string_view name, const bool& b);

  template<typename type_t>
  clon::model::root to_clon(
    std::string_view name, const type_t& val)
  {
    clon::in::model::temporary_buffer tbuf;
    to_clon(tbuf, name, val);
    return clon::parsing::parse(fmt::to_string(tbuf));
  }
}

#endif 
