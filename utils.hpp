#ifndef __clon_utils_hpp__
#define __clon_utils_hpp__

#include <string_view>
#include <algorithm>

namespace clon::utils
{
  bool between(
    const std::size_t min, 
    const std::size_t c, 
    const std::size_t max);

  bool is_digit(const char c);
  bool is_integer(std::string_view s);

  bool is_lower(const char c);
  bool is_name(std::string_view s);

  std::size_t to_integer(std::string_view v);
  
  struct tokenizer
  {
    std::string_view::const_iterator b;
    std::string_view::const_iterator e;
    const char sep;
  };

  std::size_t count(const tokenizer& t);
  std::string_view next_token(tokenizer& t);
}


#endif
