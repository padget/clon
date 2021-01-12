#ifndef __clon_hpp__
#define __clon_hpp__


#include <vector>
#include <variant>
#include <string>
#include <string_view>
#include <exception>
#include <stdexcept>
#include <fmt/format.h>
#include <istream>
#include <limits>

#include <iostream>

namespace std
{
  using boolean = bool;
  using sv = std::string_view;
  template<typename type_t>
  using const_ref = std::reference_wrapper<const type_t>;
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

  using number = double;
  using boolean = bool;
  using string = std::string;
  using object = std::vector<clon>;

  using object_cref = const object&;
  using string_cref = const string&;
  using number_cref = const number&;
  using boolean_cref = const boolean&;
  
  using clon_refs = std::vector<std::const_ref<clon>>;
  using clon_crefs = const clon_refs;

  using clon_value = std::variant<
    std::monostate, boolean,
    number, string, object>;

  struct clon
  {
    std::string name;
    clon_value val;
  };

  clon& undefined();
  clon_type type(const clon& c);

  bool is_none(const clon& c);
  bool is_bool(const clon& c);
  bool is_number(const clon& c);
  bool is_string(const clon& c);
  bool is_object(const clon& c);

  bool is_none(std::sv pth, const clon& c);
  bool is_bool(std::sv pth, const clon& c);
  bool is_number(std::sv pth, const clon& c);
  bool is_string(std::sv pth, const clon& c);
  bool is_object(std::sv pth, const clon& c);

  boolean_cref as_bool(const clon& c);
  string_cref as_string(const clon& c);
  number_cref as_number(const clon& c);
  object_cref as_object(const clon& c);

  std::string to_string(const clon& c);

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

  clon parse(std::sv s);
  clon parse_stream(std::istream& s);

  template<typename ... args>
  clon parse_fmt(std::sv pattern, args&&... as);

  const clon& get(std::sv path, const clon& c);
  clon_crefs get_all(std::sv path, const clon& c);

  const string& get_string(std::sv pth, const clon& c);
  const number& get_number(std::sv pth, const clon& c);
  const boolean& get_boolean(std::sv pth, const clon& c);
  const object& get_object(std::sv pth, const clon& c);

  const bool exists(std::sv pth, const clon& c);
  const bool check(std::sv pth, std::sv cstr, const clon& root);
}

#endif // __lon_hpp__
