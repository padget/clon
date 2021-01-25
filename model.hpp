#ifndef __clon_model_hpp__
#define __clon_model_hpp__

#include <variant>
#include <concepts>

namespace clon::model::v2
{
  template<
    typename number_t, 
    typename string_t, 
    typename boolean_t,
    typename object_t, 
    typename name_t, 
    typename buffer_t, 
    typename view_t>
  struct config
  {
    using number_type = number_t;
    using string_type = string_t;
    using object_type = object_t;
    using boolean_type = boolean_t;
    using name_type = name_t;
    using buffer_type = buffer_t;
    using view_type = view_t;
  };

  enum struct clon_type : int
  {
    none = 0,
    boolean = 1,
    number = 2,
    string = 3,
    object = 4
  };

  template<typename config_t>
  using number_type = typename config_t::number_type;

  template<typename config_t>
  using string_type = typename config_t::string_type;
  
  template<typename config_t>
  using boolean_type = typename config_t::boolean_type; 
  
  template<typename config_t>
  using object_type = typename config_t::object_type;

  template<typename config_t>
  using none_type  = std::monostate;

  template<typename config_t>
  using name_type = typename config_t::name_type;

  template<typename config_t>
  using buffer_type = typename config_t::buffer_type;

  template<typename config_t>
  using view_type = typename config_t::view_type;

  template<typename config_t>
  using clon_value = std::variant<
    none_type<config_t>, 
    boolean_type<config_t>, 
    number_type<config_t>, 
    string_type<config_t>, 
    object_type<config_t>>; 

  template<typename config_t>
  struct node
  {
    name_type<config_t> name;
    clon_value<config_t> val;
  };

  template<typename config_t>
  struct root
  {
    buffer_type<config_t> buff;
    node<config_t> root;
  };
}

namespace clon::model::v2::constraint
{
  template<typename config_t, typename type_t>
  concept possible_value =
    std::same_as<type_t, clon::model::v2::none_type<config_t>> or
    std::same_as<type_t, clon::model::v2::boolean_type<config_t>> or
    std::same_as<type_t, clon::model::v2::number_type<config_t>> or
    std::same_as<type_t, clon::model::v2::string_type<config_t>> or
    std::same_as<type_t, clon::model::v2::object_type<config_t>>;
}

namespace clon::basic::v2
{
  template<typename config_t>
  clon::model::v2::node<config_t>& undefined()
  {
    static clon::model::v2::node<config_t>{} u;
    return u; 
  }

  template<typename config_t>
  clon::model::v2::clon_type type(const clon::model::v2::node<config_t>& c)
  {
    return (clon::model::v2::clon_type) c.val.index();
  }

  template<typename config_t, clon::model::v2::constraint::possible_value<config_t> type_t>
  const bool is_(const clon::model::v2::node<config_t>& c)
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

  template<typename config_t, clon::model::v2::constraint::possible_value<config_t> type_t>
  const type_t& as_(const clon::model::v2::node<config_t>& c)
  {
    return std::get<type_t>(c.val);
  }
}




#endif 
