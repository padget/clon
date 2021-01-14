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

  template<typename type_t>
  concept number =
    std::integral<type_t> or
    std::floating_point<type_t>;

  template<typename type_t>
  concept char_sequence =
    std::same_as<type_t, std::vector<char>> or
    std::same_as<type_t, std::string> or
    std::same_as<type_t, std::sv>;
}

namespace clon
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



  using clon_refs = std::vector<std::const_ref<clon>>;
  using clon_crefs = const clon_refs;

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

  clon& undefined();
  clon_type type(const clon& c);

  template<typename type_t>
  concept value =
    std::same_as<type_t, none> or
    std::same_as<type_t, object> or
    std::same_as<type_t, string> or
    std::same_as<type_t, number> or
    std::same_as<type_t, boolean>;

  template<value type_t>
  const bool is_(const clon& c)
  {
    if constexpr (std::is_same_v<type_t, boolean>)
      return type(c) == clon_type::boolean;
    if constexpr (std::is_same_v<type_t, object>)
      return type(c) == clon_type::object;
    if constexpr (std::is_same_v<type_t, string>)
      return type(c) == clon_type::string;
    if constexpr (std::is_same_v<type_t, number>)
      return type(c) == clon_type::number;
    if constexpr (std::is_same_v<type_t, none>)
      return type(c) == clon_type::none;
  }
  
  const clon& get(std::sv path, const clon& c);
  clon_crefs get_all(std::sv path, const clon& c);

  template<value type_t>
  const bool is_(std::sv pth, const clon& c)
  {
    return is_<type_t>(get(pth, c));
  }

  template<value type_t>
  const type_t& as_(const clon& c)
  {
    return std::get<type_t>(c.val);
  }

  class expected_character : public std::invalid_argument
  {
  public:
    expected_character(std::sv chars);
  };

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

  root_clon parse(std::sv s);

  template<typename ... args>
  root_clon parse_fmt(std::sv pattern, args&&... as)
  {
    return parse(fmt::format(pattern, as...));
  }

  const clon& get(std::sv path, const clon& c);
  clon_crefs get_all(std::sv path, const clon& c);

  template<value type_t>
  const type_t& get_(std::sv pth, const clon& c)
  {
    return as_<type_t>(get(pth, c));
  }

  const bool exists(std::sv pth, const clon& c);
  const bool check(std::sv pth, std::sv cstr, const clon& root);

  std::string to_string(const clon& c);
  std::string_view to_original_string(const root_clon& c);

  using temporary_buffer = fmt::memory_buffer;

  template<typename type_t>
  const clon to_clon(const type_t& t);

  template<std::number type_t>
  void to_clon(
    temporary_buffer& tbuf,
    std::sv name, const type_t& val)
  {
    fmt::format_to(tbuf, R"(({} {}))", name, val);
  }

  template<std::char_sequence type_t>
  void to_clon(
    temporary_buffer& tbuf,
    std::sv name, const type_t& val)
  {
    fmt::format_to(tbuf, R"(({} "{}"))", name, val);
  }

  template<typename type_t>
  struct clon_native
  {
    std::sv name;
    const type_t& val;
  };

  template <typename type_t>
  clon_native<type_t> pair(
    std::sv name,
    const type_t& val)
  {
    return { name, val };
  }

  template<typename iterator_t>
  struct clon_sequence
  {
    std::sv name;
    iterator_t b;
    iterator_t e;
  };

  template<typename iterator_t>
  clon_sequence<iterator_t> sequence(
    std::sv name,
    iterator_t b,
    iterator_t e) 
  {
    return  { name, b, e };
  }

  template<typename type_t>
  constexpr bool is_native_v = false;

  template<typename type_t>
  constexpr bool is_native_v<clon_native<type_t>> = true;

  template<typename type_t>
  concept is_pair = is_native_v<type_t>;

  template<typename type_t>
  constexpr bool is_sequence_v = false;

  template<typename type_t>
  constexpr bool is_sequence_v<clon_sequence<type_t>> = true;

  template<typename type_t>
  concept is_sequence = is_sequence_v<type_t>;

  template<typename type_t>
  concept clon_convertible =
    is_pair<type_t> or
    is_sequence<type_t>;

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

#endif // __clon_hpp__
