#ifndef __clon_parsing_hpp__
#define __clon_parsing_hpp__

#include "model.hpp"
#include <exception>
#include <string_view>
#include <stdexcept>
#include <fmt/format.h>

namespace clon::parsing
{
  class expected_character : public std::invalid_argument
  {
  public:
    expected_character(std::string_view chars);
  };

  clon::model::root parse(std::string_view s);

  clon::model::root parse_fmt(std::string_view pattern, auto&&... as)
  {
    return parse(fmt::format(pattern, as...));
  }
}

#endif 
