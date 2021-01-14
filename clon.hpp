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

  template<typename type_t>
  using const_ref = std::reference_wrapper<const type_t>;

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

  struct clon;

  using none = std::monostate;
  using number = double;
  using boolean = bool;
  using string = std::string_view;
  using object = std::vector<clon>;

  using clon_value = std::variant<
    std::monostate, boolean,
    number, string, object>;

  struct clon
  {
    std::string_view name;
    clon_value val;
  };

  struct root_clon
  {
    std::buffer buff;
    clon root;
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
    std::same_as<type_t, model::none> or
    std::same_as<type_t, model::object> or
    std::same_as<type_t, model::string> or
    std::same_as<type_t, model::number> or
    std::same_as<type_t, model::boolean>;
}

// namespace clon
// {
//   using none = model::none;
//   using boolean = model::boolean;
//   using string = model::string;
//   using number = model::number;
//   using object = model::object;
//   using clon = model::clon;
//   using clon_value = model::clon_value;
//   using clon_type = model::clon_type;
//   using root_clon = model::root_clon;
// }

namespace clon::basic
{
  model::clon& undefined();
  model::clon_type type(const model::clon& c);

  template<constraint::possible_value type_t>
  const bool is_(const model::clon& c)
  {
    if constexpr (std::is_same_v<type_t, model::boolean>)
      return type(c) == model::clon_type::boolean;
    if constexpr (std::is_same_v<type_t, model::object>)
      return type(c) == model::clon_type::object;
    if constexpr (std::is_same_v<type_t, model::string>)
      return type(c) == model::clon_type::string;
    if constexpr (std::is_same_v<type_t, model::number>)
      return type(c) == model::clon_type::model::number;
    if constexpr (std::is_same_v<type_t, model::none>)
      return type(c) == model::clon_type::none;
  }

  template<constraint::possible_value type_t>
  const type_t& as_(const model::clon& c)
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

  const model::clon& get(std::sv path, const model::clon& c);
  std::vector<std::reference_wrapper<const model::clon>> get_all(
    std::sv path, const model::clon& c);

  template<constraint::possible_value type_t>
  const bool is_(std::sv pth, const model::clon& c)
  {
    return basic::is_<type_t>(get(pth, c));
  }

  template<constraint::possible_value type_t>
  const type_t& get_(std::sv pth, const model::clon& c)
  {
    return basic::as_<type_t>(get(pth, c));
  }

  const bool exists(std::sv pth, const model::clon& c);
  const bool check(std::sv pth, std::sv cstr, const model::clon& root);
}

namespace clon::parsing
{
  class expected_character : public std::invalid_argument
  {
  public:
    expected_character(std::sv chars);
  };

  model::root_clon parse(std::sv s);

  template<typename ... args>
  model::root_clon parse_fmt(std::sv pattern, args&&... as)
  {
    return parse(fmt::format(pattern, as...));
  }
}

namespace clon::out
{
  std::string to_string(const model::clon& c);
  std::string_view to_original_string(const model::root_clon& c);

}

namespace clon::in::model
{
  template<typename type_t>
  struct clon_native
  {
    std::sv name;
    const type_t& val;
  };

  template<typename iterator_t>
  struct clon_sequence
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
  constexpr bool is_native_v = false;

  template<typename type_t>
  constexpr bool is_native_v<model::clon_native<type_t>> = true;


  template<typename type_t>
  constexpr bool is_sequence_v = false;

  template<typename type_t>
  constexpr bool is_sequence_v<model::clon_sequence<type_t>> = true;
}

namespace clon::in::constraint
{
  template<typename type_t>
  concept is_pair = detail::is_native_v<type_t>;

  template<typename type_t>
  concept is_sequence = detail::is_sequence_v<type_t>;

  template<typename type_t>
  concept clon_convertible =
    is_pair<type_t> or
    is_sequence<type_t>;
}

namespace clon::in::make
{
  template <typename type_t>
  model::clon_native<type_t> native(
    std::sv name,
    const type_t& val)
  {
    return { name, val };
  }

  template<typename iterator_t>
  model::clon_sequence<iterator_t> sequence(
    std::sv name,
    iterator_t b,
    iterator_t e)
  {
    return  { name, b, e };
  }
}

namespace clon::in
{
  template<clon::constraint::number type_t>
  void to_clon(
    temporary_buffer& tbuf,
    std::sv name, const type_t& val)
  {
    fmt::format_to(tbuf, R"(({} {}))", name, val);
  }

  template<clon::constraint::char_sequence type_t>
  void to_clon(
    temporary_buffer& tbuf,
    std::sv name, const type_t& val)
  {
    fmt::format_to(tbuf, R"(({} "{}"))", name, val);
  }


  template<is_pair p>
  void to_clon_choice(
    temporary_buffer& tbuf,
    std::sv name,
    const p& ar)
  {
    to_clon(tbuf, ar.name, ar.val);
  }

  template<is_sequence seq>
  void to_clon_choice(
    temporary_buffer& tbuf,
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

  template<clon_convertible ... args_t>
  void to_clon(
    temporary_buffer& tbuf,
    std::sv name,
    const args_t& ... arg)
  {
    fmt::format_to(tbuf, "({} ", name);
    (to_clon_choice(tbuf, arg.name, arg), ...);
    fmt::format_to(tbuf, ")");
  }

  void to_clon(
    temporary_buffer& tbuf,
    std::sv name, const bool& b);

}

namespace clon
{






}

#endif // __clon_hpp__
