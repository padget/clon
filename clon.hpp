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

#include "model.hpp"
#include "path.hpp"
#include "parsing.hpp"
#include "inout.hpp"

#define CLON_VERSION 1.0.0 

namespace std
{
  using sv = std::string_view;
  using buffer = std::vector<char>;
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

  const node& undefined();
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
  const bool is_(std::sv pth, const node& c)
  {
    return is_<type_t>(get(pth, c));
  }

  template<clon::constraint::possible_value type_t>
  const type_t& get_(std::sv pth, const node& c)
  {
    return as_<type_t>(get(pth, c));
  }

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
