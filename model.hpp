#ifndef __clon_model_hpp__
#define __clon_model_hpp__

#include <string_view>
#include <vector>
#include <variant>
#include <concepts>

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
    std::string_view name;
    clon_value val;
  };

  struct root
  {
    std::vector<char> buff;
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
    std::same_as<type_t, std::string_view>;

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

#endif 
