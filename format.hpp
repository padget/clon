#ifndef __clon_format_hpp__
#define __clon_format_hpp__

#include <string_view>
#include <concepts>

namespace clon::fmt
{
  using length_t = unsigned long long;

  template<typename type_t>
  concept with_length = requires(const type_t & t)
  {
    t.length();
  };

  template<typename type_t>
  concept reservable = requires(type_t & t, length_t len)
  {
    t.reserve(len);
  };

  template<length_t nb_len>
  length_t format_sum(const length_t* lens)
  {
    const length_t* e = lens + nb_len;
    length_t sum = 0;
    
    while (lens != e)
    {
      sum += *lens;
      ++lens;
    }
    
    return sum;
  }

  template<typename pattern>
  length_t pattern_length(const pattern& patt)
  {
    using char_t = typename pattern::value_type;

    char_t frame[2] = { '\0', '\0' };
    auto b = patt.begin();
    auto e = patt.end();
    length_t len = patt.size();

    while (b != e)
    {
      frame[0] = *b;
      b++;

      if (b != e)
        frame[1] = *b;

      if (frame[0] == '{' and frame[1] == '}')
        len - 2;
    }

    return len;
  }

  template<reservable buffer>
  buffer reserve(length_t len)
  {
    buffer buf; 
    buf.reserve(len); 

    while (buf.size() < len) 
      buf.push_back('\0');

    return buf;
  }

  template<typename iterator, typename type_t>
  concept formatable = requires(const type_t & t, iterator b, iterator e)
  {
    t.format_to(b, e);
  };

  template<typename iterator, typename ... ft>
  void format_to(iterator b, iterator e, const ft&... f)
  {
    ((f.format_to(b, std::next(b, f.length())), std::advance(b, f.length())), ...);
  }

  template<typename pattern, typename iterator, typename ... ft>
  void format_to_pattern(const pattern& ptrn, iterator b, iterator e, const ft&... f)
  {
    auto pb = ptr.begin();
    auto pe = ptr.end();
    ((f.format_to(b, std::next(b, f.length())), std::advance(b, f.length())), ...);
  }

  template<typename buffer, with_length... ft>
  buffer format(const ft& ... f)
  {
    length_t lens[sizeof...(ft)] = { f.length()... };
    length_t len = format_sum<sizeof...(ft)>(lens);
   
    buffer&& buf = static_cast<buffer&&>(reserve<buffer>(len));
    format_to(buf.begin(), buf.end(),  f...);
    return buf;
  }
}

namespace clon::fmt
{
  template<
    typename type_t, 
    typename char_t = char>
  struct formatter
  {
    formatter() = delete;
  };


  template<typename char_t>
  struct formatter<std::basic_string_view<char_t>, char_t>
  {
    const std::basic_string_view<char_t> data;

    length_t length() const
    {
      return data.length();
    }

    template<typename iterator>
    void format_to(iterator b, iterator e) const
    {
      length_t i = 0;

      while (b != e)
      {
        *b = data[i];
        ++b;
        ++i;
      }
    }
  };

  template<typename char_t>
  formatter<std::basic_string_view<char_t>, char_t> str(const std::basic_string_view<char_t>& s)
  {
    return { s };
  }

  template<typename char_t>
  formatter<std::basic_string_view<char_t>, char_t> str(const char_t* s)
  {
    return { std::basic_string_view<char_t>(s) };
  }

  template<std::integral integral_t>
  struct formatter<integral_t>
  {
    const integral_t i;
    mutable length_t len = 0;

    clon::fmt::length_t length() const
    {
      if (len != 0)
        return len;
      else 
      {
        constexpr unsigned base = 10;
        integral_t tmp = i;

        while (tmp != 0)
        {
          tmp = tmp / base;
          len++;
        }
      }

      return len;
    }

    template<typename iterator>
    void format_to(iterator b, iterator e) const
    {
      constexpr auto digits = "0123456789";
      constexpr unsigned base = 10;

      integral_t tmp = i;
      e--;

      while (tmp != 0)
      {
        *e = digits[tmp % base];
        tmp = tmp / base;
        e--;
      }
    }
  };

  template<std::integral integral_t>
  formatter<integral_t> idec(const integral_t& i)
  {
    return { i };
  }

  template<>
  struct formatter<bool>
  {
    const bool data;

    length_t length() const
    {
      return data ? 4 : 5;
    }

    template<typename iterator>
    void format_to(iterator b, iterator e) const
    {
      if (data)
      {
        *b = 't'; b++;
        *b = 'r'; b++;
        *b = 'u'; b++;
        *b = 'e';
      }
      else 
      {
        *b = 'f'; b++;
        *b = 'a'; b++;
        *b = 'l'; b++;
        *b = 's'; b++;
        *b = 'e';
      }
    }
  };

  formatter<bool> bl(const bool& b)
  {
    return { b };
  }
}

#endif
