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
  array<type_t, nb> init(const type_t (&sa)[nb])
  {
    array<type_t, nb> arr;
    unsigned i = 0;

    for (type_t &t : arr)
      t = sa[i++];

    return arr;
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
  template <typename char_t>
  struct chars_span
  {
  public:
    using char_type = char_t;
    using value_type = char_t;
    using size_type = unsigned int;
    using difference_type = signed long;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using iterator = value_type *;
    using const_iterator = const value_type *;
    using reverse_iterator = clon::fmt::reverse_raiterator<iterator>;
    using const_reverse_iterator = clon::fmt::reverse_raiterator<const_iterator>;
    using position_type = size_type;

  private:
    iterator _b;
    iterator _e;

  public:
    constexpr explicit chars_span(pointer s, size_type len) : _b(s), _e(s + len) {}
    constexpr explicit chars_span(pointer b, pointer e) : chars_span(b, e - b) {}
    template <int n>
    constexpr explicit chars_span(value_type (&s)[n]) : chars_span(s, n - 1) {}
    constexpr chars_span() = default;

  public:
    constexpr iterator begin() { return _b; }
    constexpr iterator end() { return _e; }
    constexpr const_iterator begin() const { return cbegin(); }
    constexpr const_iterator end() const { return cend(); }
    constexpr const_iterator cbegin() const { return _b; }
    constexpr const_iterator cend() const { return _e; }
    constexpr reverse_iterator rbegin() { return reverse_iterator(_e - 1); }
    constexpr reverse_iterator rend() { return reverse_iterator(_b - 1); }
    constexpr const_reverse_iterator rbegin() const { return crbegin(); }
    constexpr const_reverse_iterator rend() const { return crend(); }
    constexpr const_reverse_iterator crbegin() const { return const_reverse_iterator(_e - 1); }
    constexpr const_reverse_iterator crend() const { return const_reverse_iterator(_b - 1); }

  public:
    constexpr size_type size() const { return _e - _b; }
    constexpr reference operator[](size_type i) { return *(_b + i); }
    constexpr const_reference operator[](size_type i) const { return *(_b + i); }

  public:
    constexpr chars_span subspan(size_type index, size_type n)
    {
      iterator p = index + n <= size() ? _b + index : end();
      size_type s = index + n <= size() ? n : 0;
      return chars_span(p, s);
    }

    constexpr const chars_span subspan(size_type index, size_type n) const
    {
      iterator p = index + n <= size() ? _b + index : end();
      size_type s = index + n <= size() ? n : 0;
      return chars_span(p, s);
    }

    template <typename ochar_t>
    constexpr bool equals(const chars_span<ochar_t> &o) const
    {
      if (size() == o.size())
      {
        auto b = begin(), e = end();
        auto ob = o.begin(), oe = o.end();

        while (b != e and ob != oe and *b == *ob)
        {
          ++b;
          ++ob;
        }

        return b == e and ob == oe;
      }
      else
        return false;
    }

    template <typename ochar_t>
    constexpr position_type
    find(const chars_span<ochar_t> &o, size_type start = 0) const
    {
      if (o.size() == 0)
        return size();

      if (size() >= o.size())
      {
        size_type index = start;

        while (index < size())
        {
          if (subspan(index, o.size()).equals(o))
            return index;

          ++index;
        }
      }

      return size();
    }

    template <typename ochar_t>
    constexpr const size_type count(const chars_span<ochar_t> &o) const
    {
      size_type cnt = 0;

      if (o.size() < size())
      {
        size_type start = 0;

        while (start < size())
        {
          if ((start = find(o, start)) != size())
            cnt++;

          start++;
        }
      }

      return cnt;
    }

    template <typename ochar_t>
    constexpr iterator assign(const chars_span<ochar_t> &o)
    {
      iterator b = begin();
      iterator e = end();

      if (o.size() <= size())
      {
        const_iterator ob = o.cbegin();
        const_iterator oe = o.cend();

        while (b != e and ob != oe)
          *(b++) = *(ob++);
      }

      return b;
    }

    template <int n>
    constexpr iterator assign(const value_type (&s)[n])
    {
      return assign(chars_span<const value_type>(s, n - 1));
    }
  };
} // namespace clon::fmt

namespace clon::fmt
{

  template <int n, typename char_t>
  struct chars_spans
  {
    chars_span<char_t> data[n];
  };

  template <typename char_t, int narg>
  struct pattern
  {
  private:
    chars_span<char_t> _pat;
    chars_spans<narg + 1, char_t> _inters;

  public:
    explicit pattern(chars_span<char_t> s)
        : _pat(s)
    {
      static_assert(narg > 0);
      decompose();
    }

    template <int n>
    explicit pattern(char_t (&s)[n])
        : pattern(chars_span(s, n)) {}

  public:
    const length_t size() const
    {
      length_t sum = 0;

      for (int i = 0; i < narg + 1; i++)
        sum += _inters.data[i].size();

      return sum;
    }

    const chars_spans<narg + 1, char_t> &
    spans() const
    {
      return _inters;
    }

  private:
    void decompose()
    {
      constexpr chars_span<const char_t> brackets("{}");
      length_t start = 0;
      length_t nb = 0;

      while (nb < narg + 1)
      {
        length_t found = _pat.find(brackets, start);

        auto b = _pat.begin() + start;
        auto e = _pat.begin() + found;
        _inters.data[nb] = chars_span<char_t>(b, e);

        if (found < _pat.size())
          start = found + brackets.size();

        ++nb;
      }
    }
  };
} // namespace clon::fmt

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

    array<length_t, sizeof...(ft)> &&lens = init({f.length()...});
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

#endif
