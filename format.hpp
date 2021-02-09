#ifndef __clon_format_hpp__
#define __clon_format_hpp__

#include <iostream>

#include <array>
#include <string>
#include <algorithm>
#include <numeric>
#include <string_view>

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
    type_t &get(args &&...arg) const
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
  template <typename char_t>
  using view = std::basic_string_view<char_t>;

  template <typename char_t, std::size_t n>
  using views = std::array<view<char_t>, n>;

  template <std::size_t n>
  using sizes = std::array<std::size_t, n>;

  template <typename char_t>
  using buffer = std::basic_string<char_t>;

  template <typename char_t>
  class formatter_context
  {
  private:
    buffer<char_t> &buff;

  public:
    explicit formatter_context(buffer<char_t> &_buff) : buff(_buff) {}

  public:
    void append(const char_t &c)
    {
      buff.push_back(c);
    }

    auto end()
    {
      return buff.end();
    }
  };

  template <typename char_t, std::size_t n>
  struct pattern
  {
  private:
    view<char_t> fmt;
    view<char_t> sep;
    std::size_t _parts_size = 0;
    views<char_t, n> _parts;
  public:
    explicit pattern(view<char_t> _fmt, view<char_t> _sep)
        : fmt(_fmt), sep(_sep)
    {
      auto bf = fmt.begin(), ef = fmt.end();
      auto bs = sep.begin(), es = sep.end();

      for (view<char_t> &part : _parts)
      {
        auto found = std::search(bf, ef, bs, es);
        part = view<char_t>(bf, found);

        if (found != ef)
          bf = (found + sep.size());

        _parts_size += part.size();
      }
    }

  public:
    const std::size_t &parts_size() const
    {
      return _parts_size;
    }

    std::size_t full_size() const
    {
      return fmt.size();
    }

    const views<char_t, n> &parts() const
    {
      return _parts;
    }
  };

  template <typename char_t, std::size_t n>
  std::size_t length_of(const pattern<char_t, n> &p)
  {
    return p.parts_size();
  }

  template <typename char_t>
  std::size_t length_of(const view<char_t> &v)
  {
    return v.size();
  }

  template <typename char_t>
  void format_of(
      formatter_context<char_t> &ctx,
      const view<char_t> &v)
  {
    for (const char_t &c : v)
      ctx.append(c);
  }

  template <std::integral integral_t>
  std::size_t length_of(const integral_t &i)
  {
    constexpr unsigned base(10);
    std::size_t len(0);
    integral_t tmp(i);

    while (tmp != 0)
    {
      tmp = tmp / base;
      len++;
    }

    return len;
  }

  template <typename char_t, std::integral integral_t>
  void format_of(
      formatter_context<char_t> &ctx,
      const integral_t &t)
  {
    integral_t tmp(t);
    std::size_t cnt(0);

    while (tmp != 0)
    {
      ctx.append("0123456789"[tmp % 10]);
      tmp = tmp / 10;
      cnt++;
    }

    std::reverse(ctx.end() - cnt, ctx.end());
  }

  template <typename first_t, typename second_t, typename... tails_t>
  std::size_t length_of(const first_t &f, const second_t &s, const tails_t &...t)
  {
    return length_of(f) + (length_of(s) + ... + length_of(t));
  }

  template <typename char_t, std::size_t n>
  struct formatter
  {
    constexpr static view<char_t> sep = "{}";
    pattern<char_t, n + 1> p;

  public:
    explicit formatter(view<char_t> fmt) : p(fmt, sep) {}

  public:
    template <typename... args_t>
    buffer<char_t> format(const args_t &...args)
    {
      static_assert(sizeof...(args_t) == n);

      buffer<char_t> buff;
      buff.reserve(length_of(p, args...));

      formatter_context<char_t> ctx{buff};
      std::size_t i(0);
      ((format_of(ctx, p.parts().at(i++)), format_of(ctx, args)), ...);
      format_of(ctx, p.parts().at(i));

      return buff;
    }
  };

  namespace detail
  {
    template <typename char_t, typename... args_t>
    std::size_t predict_length_of(
        std::basic_string_view<char_t> fmt,
        const args_t &...args)
    {
      constexpr std::size_t nb_args = sizeof...(args_t);
      constexpr view<char_t> sep = "{}";
      pattern<char_t, nb_args + 1> p(fmt, sep);
      return length_of(p, args...);
    }

    template <typename char_t, typename... args_t>
    void format_into(
        formatter_context<char_t> &ctx,
        view<char_t> fmt,
        const args_t &...args)
    {
      constexpr std::size_t nb_args = sizeof...(args_t);
      constexpr view<char_t> sep = "{}";
      pattern<char_t, nb_args + 1> p(fmt, sep);
      std::size_t i(0);
      ((format_of(ctx, p.parts().at(i++)), format_of(ctx, args)), ...);
      format_of(ctx, p.parts().at(i));
    }
  } // namespace detail

  template <typename... args_t>
  buffer<char> format(
      std::basic_string_view<char> fmt,
      const args_t &...args)
  {
    formatter<char, sizeof...(args_t)> f(fmt);
    return f.format(args...);
  }

  template <typename... args_t>
  buffer<wchar_t> format(
      std::basic_string_view<wchar_t> fmt,
      const args_t &...args)
  {
    formatter<wchar_t, sizeof...(args_t)> f(fmt);
    return f.format(args...);
  }

  template <typename... args_t>
  std::size_t predict_length_of(
      std::basic_string_view<char> fmt,
      const args_t &...args)
  {
    return detail::predict_length_of(fmt, args...);
  }

  template <typename... args_t>
  std::size_t predict_length_of(
      std::basic_string_view<wchar_t> fmt,
      const args_t &...args)
  {
    return detail::predict_length_of(fmt, args...);
  }

  template <typename... args_t>
  void format_into(
      formatter_context<char> &ctx,
      std::basic_string_view<char> fmt,
      const args_t &...args)
  {
    detail::format_into(ctx, fmt, args...);
  }

  template <typename... args_t>
  void format_into(
      formatter_context<wchar_t> &ctx,
      std::basic_string_view<wchar_t> fmt,
      const args_t &...args)
  {
    detail::format_into(ctx, fmt, args...);
  }

} // namespace clon::fmt

#endif
