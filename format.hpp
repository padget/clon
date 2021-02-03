#ifndef __clon_format_hpp__
#define __clon_format_hpp__
#include <iostream>

namespace clon::fmt
{
  using length_t = unsigned long long;
}

namespace clon::fmt
{
  template <typename type_t, typename function_t>
  class once
  {
  private:
    mutable type_t data;
    mutable bool initialized = false;

  public:
    template <typename... args>
    type_t &get(args &&... arg) const
    {
      if (not initialized)
      {
        data = function_t{}(static_cast<args &&>(arg)...);
        initialized = true;
      }

      return data;
    }
  };
} // namespace clon::fmt

namespace clon::fmt
{
  template <typename raiterator>
  class reverse_raiterator;

  template <typename value_t>
  class reverse_raiterator<value_t *>
  {
  public:
    using iterator_type = value_t *;
    using value_type = value_t;
    using difference_type = unsigned long long;
    using pointer = value_t *;
    using reference = value_t &;

  private:
    iterator_type _base;

  public:
    constexpr reverse_raiterator() = default;
    constexpr explicit reverse_raiterator(iterator_type it) : _base{it} {}

    constexpr iterator_type base() const { return _base; }
    constexpr reference operator*() const { return *_base; }

    constexpr bool operator==(const reverse_raiterator &o)
    {
      return _base == o._base;
    }

    constexpr bool operator!=(const reverse_raiterator &o)
    {
      return not operator==(o);
    }

    constexpr reverse_raiterator &operator++()
    {
      --_base;
      return *this;
    }

    constexpr reverse_raiterator operator++(int)
    {
      reverse_raiterator tmp(*this);
      operator++();
      return tmp;
    }

    constexpr reverse_raiterator &operator--()
    {
      ++_base;
      return *this;
    }

    constexpr reverse_raiterator operator--(int)
    {
      reverse_raiterator tmp(*this);
      operator--();
      return tmp;
    }

    constexpr reverse_raiterator operator+(difference_type n) const
    {
      return reverse_raiterator(_base - n);
    }

    constexpr reverse_raiterator &operator+=(difference_type n)
    {
      _base -= n;
      return *this;
    }
    constexpr reverse_raiterator operator-(difference_type n) const
    {
      return reverse_raiterator(_base + n);
    }

    constexpr reverse_raiterator &operator-=(difference_type n)
    {
      _base += n;
      return *this;
    }
  };
} // namespace clon::fmt

namespace clon::fmt
{
  template <typename type_t, unsigned nb>
  struct array
  {
    type_t data[nb];
  };

  template <typename type_t, unsigned nb>
  array<type_t, nb> init_array(const type_t (&sa)[nb])
  {
    array<type_t, nb> arr;
    unsigned i = 0;

    for (type_t &t : arr)
      t = sa[i++];

    return arr;
  }

  template <typename type_t, unsigned nb>
  type_t *begin(array<type_t, nb> &arr)
  {
    return arr.data;
  }

  template <typename type_t, unsigned nb>
  type_t *end(array<type_t, nb> &arr)
  {
    return begin(arr) + nb;
  }

  template <typename type_t, unsigned nb>
  const type_t *begin(const array<type_t, nb> &arr)
  {
    return arr.data;
  }

  template <typename type_t, unsigned nb>
  const type_t *end(const array<type_t, nb> &arr)
  {
    return begin(arr) + nb;
  }

  template <typename type_t, unsigned nb>
  const unsigned size(const array<type_t, nb> &)
  {
    return nb;
  }

  template <typename type_t, unsigned nb>
  type_t accumulate(const array<type_t, nb> &arr, const type_t &start)
  {
    type_t s = start;

    for (const type_t &t : arr)
      s += t;

    return s;
  }
} // namespace clon::fmt

namespace clon::fmt
{
  template <typename type_t>
  struct span
  {
    type_t *b;
    type_t *e;
  };

  template <typename type_t>
  span<const type_t> init_span(const type_t *b, const type_t *e)
  {
    return {b, e};
  }

  template <typename type_t>
  span<type_t> init_span(type_t *b, type_t *e)
  {
    return {b, e};
  }

  template <typename type_t, unsigned nb>
  span<const type_t> init_span(const type_t (&sa)[nb])
  {
    return init_span(sa, sa + nb);
  }

  template <typename type_t, unsigned nb>
  span<const type_t> init_cspan(const type_t (&sa)[nb])
  {
    return init_span(sa, sa + nb - 1);
  }

  template <typename type_t>
  type_t *begin(span<type_t> &spa)
  {
    return spa.b;
  }

  template <typename type_t>
  type_t *end(span<type_t> &spa)
  {
    return spa.e;
  }

  template <typename type_t>
  const type_t *begin(const span<type_t> &spa)
  {
    return spa.b;
  }

  template <typename type_t>
  const type_t *end(const span<type_t> &spa)
  {
    return spa.e;
  }

  template <typename type_t>
  const unsigned size(const span<type_t> &spa)
  {
    return spa.e - spa.b;
  }

  template <typename type_t>
  span<type_t> subspan(span<type_t> &s, unsigned start, unsigned sz)
  {
    auto b = start <= size(s) ? begin(s) + start : end(s);
    auto e = start + sz <= size(s) ? b + sz : end(s);
    return init_span(b, e);
  }

  template <typename type_t>
  const span<type_t> subspan(const span<type_t> &s, unsigned start, unsigned sz)
  {
    auto b = start <= size(s) ? begin(s) + start : end(s);
    auto e = start + sz <= size(s) ? b + sz : end(s);
    return init_span(b, e);
  }

  template <typename cont_t>
  const bool equals(
      const cont_t &c1,
      const cont_t &c2)
  {
    auto b1 = begin(c1), e1 = end(c1);
    auto b2 = begin(c2), e2 = end(c2);

    while (b1 != e1 and b2 != e2 and *b1 == *b2)
    {
      ++b1;
      ++b2;
    }

    return b1 == e1 and b2 == e2;
  }

  template <
      typename type_t,
      typename otype_t>
  unsigned find(
      const span<type_t> &s,
      const span<otype_t> &o,
      unsigned start = 0)
  {
    unsigned os = size(o);
    unsigned ss = size(s);

    if (os == 0)
      return ss;

    if (ss >= os)
    {
      unsigned index = start;

      while (index < ss)
      {
        auto &&sub = subspan(s, index, os);

        if (equals(sub, o))
          return index;

        ++index;
      }
    }

    return ss;
  }

  template <unsigned n, typename type_t, typename otype_t>
  array<span<type_t>, n> split_n(const span<type_t> &src, const span<otype_t> &sep)
  {
    array<span<type_t>, n> splitted;
    const auto src_begin = begin(src);
    const auto src_size = size(src);
    const auto sep_size = size(sep);
    unsigned start = 0u;

    for (span<type_t> &s : splitted)
    {
      unsigned found = find(src, sep, start);
      s.b = src_begin + start;
      s.e = src_begin + found;

      if (found < src_size)
        start = found + sep_size;
    }

    return splitted;
  }
} // namespace clon::fmt

/*
namespace clon::fmt
{
  template <typename type_t>
  concept with_length = requires(const type_t &t)
  {
    t.length();
  };

  template <typename type_t>
  concept reservable = requires(type_t &t, length_t len)
  {
    t.reserve(len);
  };

  template <reservable buffer>
  buffer reserve(length_t len)
  {
    buffer buf;
    buf.reserve(len);

    while (buf.size() < len)
      buf.push_back('o');

    return buf;
  }

  template <typename pchar_t, typename char_t, typename... ft>
  void format_to(
      const pattern<pchar_t, sizeof...(ft)> &p,
      chars_span<char_t> cspan,
      const ft &... f)
  {
    auto b = cspan.begin();

    ((f.format_to(chars_span<char_t>(b, f.length())), b += f.length()), ...);
  }

  template <
      reservable buffer,
      typename pchar_t,
      int pn,
      with_length... ft>
  buffer format(const pchar_t (&p)[pn], const ft &... f)
  {
    using char_t = typename buffer::value_type;
    using pattern_t = pattern<const pchar_t, sizeof...(ft)>;

    array<length_t, sizeof...(ft)> &&lens = init_array({f.length()...});
    buffer &&buf = reserve<buffer>(lens.sum());
    chars_span<char_t> cspan(&*buf.begin(), buf.size());
    pattern_t patt(chars_span<const pchar_t>(p, pn));
    format_to(patt, cspan, f...);
    return buf;
  }
} // namespace clon::fmt

namespace clon::fmt
{
  template <
      typename type_t,
      typename lengther_t,
      typename converter_t>
  struct formatter
  {
    const type_t *t;
    once<length_t, lengther_t> len;

    constexpr length_t length() const
    {
      return len.get(*t);
    }

    template <typename char_t>
    constexpr void format_to(chars_span<char_t> cspan) const
    {
      converter_t{}(*t, cspan);
    }
  };

  template <
      typename type_t,
      typename lengther_t,
      typename format_to_t>
  formatter<type_t, lengther_t, format_to_t>
  make_formatter(const type_t &t, lengther_t &&, format_to_t &&)
  {
    return {&t};
  }
} // namespace clon::fmt

namespace clon::fmt
{
  struct cstr_length
  {
    template <typename char_t, int n>
    constexpr length_t operator()(
        const char_t (&s)[n]) const
    {
      return n - 1;
    }
  };

  struct cstr_format_to
  {
    template <typename char_t, int n>
    constexpr void operator()(
        const char_t (&s)[n],
        chars_span<char_t> &buf) const
    {
      buf.assign(s);
    }
  };

  template <typename char_t, int n>
  auto cstr(const char_t (&s)[n])
  {
    using l = cstr_length;
    using f = cstr_format_to;

    return make_formatter(s, l{}, f{});
  }

  struct str_length
  {
    template <typename str_t>
    length_t operator()(const str_t &s)
    {
      return s.size();
    }
  };

  struct str_format_to
  {
    template <typename str_t, typename char_t>
    void operator()(
        const str_t &s,
        chars_span<char_t> &buf)
    {
      buf.assign(chars_span<const char_t>(s.data(), s.size()));
    }
  };

  template <typename str_t>
  auto str(const str_t &s)
  {
    using l = str_length;
    using f = str_format_to;

    return make_formatter(s, l{}, f{});
  }

  struct integral_length
  {
    template <typename integral_t>
    constexpr length_t operator()(
        const integral_t &i) const
    {
      constexpr unsigned base = 10;
      integral_t tmp = i;
      length_t len = 0;

      while (tmp != 0)
      {
        tmp = tmp / base;
        len++;
      }

      return len;
    }
  };

  struct integral_format_to
  {
    template <typename integral_t, typename char_t>
    constexpr void operator()(
        const integral_t &i,
        chars_span<char_t> &buf) const
    {
      constexpr auto digits = "0123456789";
      constexpr unsigned base = 10;
      integral_t tmp = i;

      auto b = buf.rbegin();

      while (tmp != 0)
      {
        *b = digits[tmp % base];
        tmp = tmp / base;
        ++b;
      }
    }
  };

  template <typename integral_t>
  auto idec(const integral_t &i)
  {
    return make_formatter(
        i, integral_length{}, integral_format_to{});
  }

  struct bool_length
  {
    constexpr length_t operator()(
        const bool &b) const
    {
      return b ? 4 : 5;
    }
  };

  struct bool_format_to
  {
    template <typename char_t>
    constexpr void operator()(
        const bool &b,
        chars_span<char_t> &buf) const
    {
      if (b)
        buf.assign("true");
      else
        buf.assign("false");
    }
  };

  auto bl(const bool &b)
  {
    using l = bool_length;
    using f = bool_format_to;

    return make_formatter(b, l{}, f{});
  }

} // namespace clon::fmt
*/
#endif
