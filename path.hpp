#ifndef __clon_path_hpp__
#define __clon_path_hpp__

#include "model.hpp"
#include <vector>
#include <stdexcept>
#include <string_view>

namespace clon::path
{
  class malformed_path : public std::invalid_argument
  {
  public:
    malformed_path(std::string_view reason);
  };

  class malformed_number : public malformed_path
  {
  public:
    malformed_number(std::string_view path);
  };

  class malformed_name : public malformed_path
  {
  public:
    malformed_name(std::string_view path);
  };

  class unreachable_path : public std::invalid_argument
  {
  public:
    unreachable_path(std::string_view pth);
  };

  class malformed_constraint
    : public std::invalid_argument
  {
  public:
    malformed_constraint(std::string_view reason);
  };

  const clon::model::node& get(std::string_view path, const clon::model::node& c);
  const std::vector<std::reference_wrapper<const clon::model::node>> get_all(
    std::string_view pth, const clon::model::node& c);

  template<clon::constraint::possible_value type_t>
  const bool is_(std::string_view pth, const clon::model::node& c)
  {
    return clon::basic::is_<type_t>(get(pth, c));
  }

  template<clon::constraint::possible_value type_t>
  const type_t& get_(std::string_view pth, const clon::model::node& c)
  {
    return clon::basic::as_<type_t>(get(pth, c));
  }

  const bool check(std::string_view pth, std::string_view cstr, const clon::model::node& root);
}

#endif
