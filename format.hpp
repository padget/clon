#ifndef __clon_format_hpp__
#define __clon_format_hpp__

#include <string_view>

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

  template<reservable res>
  res reserve(length_t len)
  {
    res r; r.reserve(len); return r;
  }

  template<typename res, typename type_t>
  concept formatable = requires(const type_t & t, res & r)
  {
    t.format_to(r);
  };

  template<typename res, typename ... ft>
  void format_to(res& r, const ft&... f)
  {
    (f.format_to(r), ...);
  }

  template<typename res, with_length... ft>
  res format(const ft& ... f)
  {
    length_t lens[sizeof...(ft)] = { f.length()... };
    length_t len = format_sum<sizeof...(ft)>(lens);
   // length_t plen = pattern_length(patt);
    res r = static_cast<res&&>(reserve<res>(/*plen + */len));
    format_to(r, /*patt,*/ f...);
    return r;
  }
}

namespace clon::fmt
{
  template<typename string_view>
  struct string_format
  {
    const string_view& data;

    length_t length() const
    {
      return data.length();
    }

    template<typename buffer>
    void format_to(buffer& s) const
    {
      s.append(data);
    }
  };

  string_format<std::string_view> str(const std::string_view& s)
  {
    return { s };
  }

  template<typename integral>
  struct integral_format
  {
    const integral& i;

    clon::fmt::length_t length() const
    {
      constexpr unsigned base = 10;
      int count = 0;
      int tmp = i;

      while (tmp != 0)
      {
        tmp = tmp / base;
        count++;
      }

      return count;
    }

    template<typename buffer>
    void format_to(buffer& s) const
    {
      constexpr auto digits = "0123456789";
      constexpr unsigned base = 10;

      int tmp = i;
      int count = 1;
      s.append(length(), '0');

      while (tmp != 0)
      {
        s.at(s.size() - count) = digits[tmp % base];
        tmp = tmp / base;
        count++;
      }
    }
  };

  template<typename integral>
  integral_format<integral> idec(const integral& i)
  {
    return { i };
  }

  struct boolean_format
  {
    const bool& data;

    inline length_t length() const
    {
      return data ? 4 : 5;
    }

    template<typename buffer>
    void format_to(buffer& s) const
    {
      s.append(data ? "true" : "false");
    }
  };

  boolean_format bl(const bool& b)
  {
    return { b };
  }
}

#endif
