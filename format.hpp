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

  template <typename type_t>
  class basic_format
  {
    const type_t *data = nullptr;
    mutable std::size_t len = 0;

  public:
    explicit basic_format(const type_t &_data)
        : data(&_data) {}

    const std::size_t &length() const
    {
      if (len == 0)
        len = length_of(*data);

      return len;
    }

    template <typename char_t>
    void format_into(formatter_context<char_t> &ctx) const
    {
      format_of(ctx, *data);
    }
  };

  template <typename type_t>
  basic_format<type_t> make_format(const type_t &data)
  {
    return basic_format<type_t>(data);
  }

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

  template <typename char_t, std::size_t n>
  class partial_formatter
  {
    constexpr static view<char_t> sep = "{}";
    pattern<char_t, n + 1> p;

  public:
    explicit partial_formatter(view<char_t> fmt) : p(fmt, sep) {}

  public:
    template <typename... args_t>
    const std::size_t length(const basic_format<args_t> &...args) const
    {
      return (args.length() + ... + p.parts_size());
    }

    template <typename... args_t>
    void format(formatter_context<char_t> &ctx, const basic_format<args_t> &...args) const
    {
      std::size_t i(0);
      ((make_format(p.parts().at(i++)).format_into(ctx), args.format_into(ctx)), ...);
      make_format(p.parts().at(i)).format_into(ctx);
    }
  };

  template <typename char_t, std::size_t n>
  class formatter
  {
    partial_formatter<char_t, n> partial;

  public:
    explicit formatter(view<char_t> fmt) : partial(fmt) {}

  public:
    template <typename... args_t>
    buffer<char_t> format(const basic_format<args_t> &...args)
    {
      static_assert(sizeof...(args_t) == n);

      buffer<char_t> buff;
      buff.reserve(partial.length(args...));

      formatter_context<char_t> ctx{buff};
      partial.format(ctx, args...);

      return buff;
    }
  };

  template <typename... args_t>
  buffer<char> format(
      std::basic_string_view<char> fmt,
      const args_t &...args)
  {
    formatter<char, sizeof...(args_t)> f(fmt);
    return f.format(make_format(args)...);
  }

  template <typename... args_t>
  buffer<wchar_t> format(
      std::basic_string_view<wchar_t> fmt,
      const args_t &...args)
  {
    formatter<wchar_t, sizeof...(args_t)> f(fmt);
    return f.format(make_format(args)...);
  }

  template <typename... args_t>
  std::size_t predict_length_of(
      std::basic_string_view<char> fmt,
      const args_t &...args)
  {
    partial_formatter<char, sizeof...(args_t)> partial(fmt);
    return partial.length(make_format(args)...);
  }

  template <typename... args_t>
  std::size_t predict_length_of(
      std::basic_string_view<wchar_t> fmt,
      const args_t &...args)
  {
    partial_formatter<wchar_t, sizeof...(args_t)> partial(fmt);
    return partial.length(make_format(args)...);
  }

  template <typename... args_t>
  void format_into(
      formatter_context<char> &ctx,
      std::basic_string_view<char> fmt,
      const args_t &...args)
  {
    partial_formatter<char, sizeof...(args_t)> partial(fmt);
    return partial.format(ctx, make_format(args)...);
  }

  template <typename... args_t>
  void format_into(
      formatter_context<wchar_t> &ctx,
      std::basic_string_view<wchar_t> fmt,
      const args_t &...args)
  {
    partial_formatter<wchar_t, sizeof...(args_t)> partial(fmt);
    return partial.format(ctx, make_format(args)...);
  }

} // namespace clon::fmt

#endif
