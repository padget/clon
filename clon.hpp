#ifndef __clon_hpp__
#define __clon_hpp__

#include <exception>
#include <iostream>
#include <istream>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <concepts>

#include <fmt/format.h>

#define CLON_VERSION 1.0.0 

namespace std
{
  using sv = std::string_view;
  using buffer = std::vector<char>;
}

namespace clon::model
{
  enum struct clon_type : int
  {
    none = 0,
    boolean = 1,
    number = 2,
    string = 3,
    object = 4,
  };

  struct node;

  using none = std::monostate;
  using number = double;
  using boolean = bool;
  using string = std::string_view;
  using object = std::vector<node>;

  using clon_value = std::variant<
    std::monostate, boolean,
    number, string, object>;

  struct node
  {
    std::sv name;
    clon_value val;
  };

  struct root
  {
    std::buffer buff;
    node root;
  };
}

namespace clon::constraint
{
  template<typename type_t>
  concept number =
    std::integral<type_t> or
    std::floating_point<type_t>;

  template<typename type_t>
  concept char_sequence =
    std::same_as<type_t, std::vector<char>> or
    std::same_as<type_t, std::string> or
    std::same_as<type_t, std::sv>;

  template<typename type_t>
  concept possible_value =
    std::same_as<type_t, clon::model::none> or
    std::same_as<type_t, clon::model::object> or
    std::same_as<type_t, clon::model::string> or
    std::same_as<type_t, clon::model::number> or
    std::same_as<type_t, clon::model::boolean>;
}

namespace clon::basic
{
  clon::model::node& undefined();
  clon::model::clon_type type(const clon::model::node& c);

  template<clon::constraint::possible_value type_t>
  const bool is_(const clon::model::node& c)
  {
    if constexpr (std::is_same_v<type_t, clon::model::boolean>)
      return type(c) == clon::model::clon_type::boolean;
    if constexpr (std::is_same_v<type_t, clon::model::object>)
      return type(c) == clon::model::clon_type::object;
    if constexpr (std::is_same_v<type_t, clon::model::string>)
      return type(c) == clon::model::clon_type::string;
    if constexpr (std::is_same_v<type_t, clon::model::number>)
      return type(c) == clon::model::clon_type::number;
    if constexpr (std::is_same_v<type_t, clon::model::none>)
      return type(c) == clon::model::clon_type::none;
  }

  template<clon::constraint::possible_value type_t>
  const type_t& as_(const clon::model::node& c)
  {
    return std::get<type_t>(c.val);
  }
}

namespace clon::path
{
  class malformed_path : public std::invalid_argument
  {
  public:
    malformed_path(std::sv reason);
  };

  class malformed_number : public malformed_path
  {
  public:
    malformed_number(std::sv path);
  };

  class malformed_name : public malformed_path
  {
  public:
    malformed_name(std::sv path);
  };

  class unreachable_path : public std::invalid_argument
  {
  public:
    unreachable_path(std::sv pth);
  };

  class malformed_constraint
    : public std::invalid_argument
  {
  public:
    malformed_constraint(std::sv reason);
  };

  const clon::model::node& get(std::sv path, const clon::model::node& c);
  const std::vector<std::reference_wrapper<const clon::model::node>> get_all(
    std::sv pth, const clon::model::node& c);

  template<clon::constraint::possible_value type_t>
  const bool is_(std::sv pth, const clon::model::node& c)
  {
    return clon::basic::is_<type_t>(get(pth, c));
  }

  template<clon::constraint::possible_value type_t>
  const type_t& get_(std::sv pth, const clon::model::node& c)
  {
    return clon::basic::as_<type_t>(get(pth, c));
  }

  const bool check(std::sv pth, std::sv cstr, const clon::model::node& root);
}

namespace clon::parsing
{
  class expected_character : public std::invalid_argument
  {
  public:
    expected_character(std::sv chars);
  };

  clon::model::root parse(std::sv s);

  clon::model::root parse_fmt(std::sv pattern, auto&&... as)
  {
    return parse(fmt::format(pattern, as...));
  }
}

namespace clon::out
{
  std::string to_string(const clon::model::node& c);
  std::sv to_original_string(const clon::model::root& c);
}

namespace clon::in::model
{
  template<typename type_t>
  struct in_pair
  {
    std::sv name;
    const type_t& val;
  };

  template<typename iterator_t>
  struct in_sequence
  {
    std::sv name;
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
    std::sv name, const type_t& val)
  {
    return { name, val };
  }

  template<typename iterator_t>
  clon::in::model::in_sequence<iterator_t> sequence(
    std::sv name, iterator_t b, iterator_t e)
  {
    return  { name, b, e };
  }
}

namespace clon::in
{
  template<clon::constraint::number type_t>
  void to_clon(
    clon::in::model::temporary_buffer& tbuf,
    std::sv name, const type_t& val)
  {
    fmt::format_to(tbuf, R"(({} {}))", name, val);
  }

  template<clon::constraint::char_sequence type_t>
  void to_clon(
    clon::in::model::temporary_buffer& tbuf,
    std::sv name, const type_t& val)
  {
    fmt::format_to(tbuf, R"(({} "{}"))", name, val);
  }

  template<clon::in::constraint::is_pair p >
  void to_clon_choice(
    clon::in::model::temporary_buffer& tbuf,
    std::sv name,
    const p& ar)
  {
    to_clon(tbuf, ar.name, ar.val);
  }

  template<clon::in::constraint::is_sequence seq>
  void to_clon_choice(
    clon::in::model::temporary_buffer& tbuf,
    std::sv name,
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
    std::sv name,
    const args_t& ... arg)
  {
    fmt::format_to(tbuf, "({} ", name);
    (to_clon_choice(tbuf, arg.name, arg), ...);
    fmt::format_to(tbuf, ")");
  }

  void to_clon(
    clon::in::model::temporary_buffer& tbuf,
    std::sv name, const bool& b);

  template<typename type_t>
  clon::model::root to_clon(
    std::sv name, const type_t& val)
  {
    clon::in::model::temporary_buffer tbuf;
    to_clon(tbuf, name, val);
    return clon::parsing::parse(fmt::to_string(tbuf));
  }
}

namespace clon
{
  using node = clon::model::node;
  using root = clon::model::root;
  using clon_type = clon::model::clon_type;

  using none = clon::model::none;
  using boolean = clon::model::boolean;
  using number = clon::model::number;
  using string = clon::model::string;
  using object = clon::model::object;

  node& undefined();
  clon_type type(const node& c);

  template<clon::constraint::possible_value type_t>
  const bool is_(const node& c)
  {
    return clon::basic::is_<type_t>(c);
  }

  template<clon::constraint::possible_value type_t>
  const type_t& as_(const clon::model::node& c)
  {
    return clon::basic::as_<type_t>(c);
  }

  using malformed_path = clon::path::malformed_path;
  using malformed_number = clon::path::malformed_number;
  using malformed_name = clon::path::malformed_name;
  using unreachable_path = clon::path::unreachable_path;
  using malformed_constraint = clon::path::malformed_constraint;

  const node& get(std::sv path, const node& c);
  std::vector<std::reference_wrapper<const node>> get_all(
    std::sv path, const node& c);

  template<clon::constraint::possible_value type_t>
  const bool is_(std::sv pth, const node& c);

  template<clon::constraint::possible_value type_t>
  const type_t& get_(std::sv pth, const node& c);

  const bool check(std::sv pth, std::sv cstr, const node& root);

  using expected_character = clon::parsing::expected_character;

  root parse(std::sv s);
  root parse_fmt(std::sv pattern, auto&&... as);

  std::string to_string(const node& c);
  std::sv to_original_string(const root& c);

  template<typename type_t>
  using in_pair = clon::in::model::in_pair<type_t>;
  template<typename iterator_t>
  using in_sequence = clon::in::model::in_sequence<iterator_t>;
  using temporary_buffer = clon::in::model::temporary_buffer;

  template <typename type_t>
  in_pair<type_t> pair(
    std::sv name, const type_t& val)
  {
    return clon::in::make::pair(name, val);
  }

  template<typename iterator_t>
  in_sequence<iterator_t> sequence(
    std::sv name, iterator_t b, iterator_t e)
  {
    return clon::in::make::sequence(name, b, e);
  }
}

#endif // __clon_hpp__
