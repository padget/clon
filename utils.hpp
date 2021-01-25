#ifndef __clon_utils_hpp__
#define __clon_utils_hpp__

#include <string_view>

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

  bool operator==(const tokenizer& t1, const tokenizer& t2);

  tokenizer begin(const tokenizer& t);
  tokenizer end(const tokenizer& t);

  tokenizer& operator++(tokenizer& t);
  tokenizer operator++(tokenizer& t, int);

  std::string_view operator*(const tokenizer& t);

  template<typename iterator, auto mapper>
  struct transformer
  {
    iterator b;
    iterator e;
  };

  template<typename iterator, auto map>
  bool operator==(
    const transformer<iterator, map>& t1,
    const transformer<iterator, map>& t2)
  {
    return t1.b == t2.b and t1.e == t2.e and t1.sep == t2.sep;
  }

  template<typename iterator, auto map>
  transformer<iterator, map> begin(const transformer<iterator, map>& t)
  {
    return t;
  }

  template<typename iterator, auto map>
  transformer<iterator, map> end(const transformer<iterator, map>& t)
  {
    return { t.e, t.e };
  }


  template<typename iterator, auto map>
  transformer<iterator, map>& operator++(transformer<iterator, map>& t)
  {
    t.b++;
    return t;
  }

  template<typename iterator, auto map>
  transformer<iterator, map> operator++(transformer<iterator, map>& t, int)
  {
    transformer<iterator, map> tmp = t;
    operator++(t);
    return tmp;
  }

  template<typename iterator, auto map>
  std::string_view operator*(const transformer<iterator, map>& t)
  {
    return map(*t.b);
  }
}


#endif
