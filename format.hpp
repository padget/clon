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
  struct formatter_context
  {
    char_t *data;
    std::size_t len;

    formatter_context<char_t>
    subcontext(std::size_t offset, std::size_t len)
    {
      return {data + offset, len};
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
    unsigned i(0);

    for (const char_t &c : v)
      ctx.data[i++] = c;
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
    integral_t tmp = t;
    char_t *rb = ctx.data + ctx.len - 1;

    while (tmp != 0)
    {
      *(rb--) = "0123456789"[tmp % 10];
      tmp = tmp / 10;
    }
  }

  namespace detail
  {
    template <std::size_t n, typename char_t>
    views<char_t, n> split_n(view<char_t> fmt, view<char_t> sep)
    {
      views<char_t, n> parts;

      auto bf = fmt.begin(), ef = fmt.end();
      auto bs = sep.begin(), es = sep.end();

      for (view<char_t> &part : parts)
      {
        auto found = std::search(bf, ef, bs, es);
        part = view<char_t>(bf, found);

        if (found != ef)
          bf = (found + sep.size());
      }

      return parts;
    }

    template <typename char_t, std::size_t n>
    sizes<n> fmt_sizes(const views<char_t, n> &fmt_parts)
    {
      sizes<n> ss;

      for (std::size_t i(0); i < n; ++i)
        ss[i] = fmt_parts[i].size();

      return ss;
    }

    template <typename char_t, typename... args_t>
    buffer<char_t> format(view<char_t> fmt, const args_t &...args)
    {
      // TODO 00001 : see to factorize this with  00002
      constexpr std::size_t nb_args = sizeof...(args_t);
      constexpr view<char_t> sep = "{}";

      views<char_t, nb_args + 1> fmt_parts = split_n<nb_args + 1>(fmt, sep);

      sizes<nb_args + 1> fs = fmt_sizes(fmt_parts);
      sizes<nb_args> ts = {length_of(args)...};

      std::size_t fts = std::accumulate(fs.begin(), fs.end(), 0);
      std::size_t tts = std::accumulate(ts.begin(), ts.end(), 0);
      std::size_t buff_len = fts + tts;

      buffer<char_t> buff(buff_len, (char_t)0);
      char_t *data(buff.data());
      std::size_t i(0);
      std::size_t offset(0);
      formatter_context<char_t> ctx;

      ((
           ctx.data = data + offset, ctx.len = fs[i],
           format_of(ctx, fmt_parts.at(i)),
           offset += fs[i],
           ctx.data = data + offset, ctx.len = ts[i],
           format_of(ctx, args),
           offset += ts[i],
           i++),
       ...);

      ctx.data = data + offset;
      ctx.len = fs[i];
      format_of(ctx, fmt_parts[i]);

      return buff;
    }

    template <typename char_t, typename... args_t>
    std::size_t predict_length_of(
        std::basic_string_view<char_t> fmt,
        const args_t &...args)
    {
      // TODO 00002 : see to factorize this with 00001
      constexpr std::size_t nb_args = sizeof...(args_t);
      constexpr view<char_t> sep = "{}";
      views<char_t, nb_args + 1> fmt_parts = split_n<nb_args + 1>(fmt, sep);

      sizes<nb_args + 1> fs = fmt_sizes(fmt_parts);
      sizes<nb_args> ts = {length_of(args)...};

      std::size_t fts = std::accumulate(fs.begin(), fs.end(), 0);
      std::size_t tts = std::accumulate(ts.begin(), ts.end(), 0);
      return fts + tts;
    }

    template <typename char_t, typename... args_t>
    void format_into(
        formatter_context<char_t> &ctx,
        view<char_t> fmt,
        const args_t &...args)
    {
      // TODO 00003 : see to factorize this with 00002 and 00001
      constexpr std::size_t nb_args = sizeof...(args_t);
      constexpr view<char_t> sep = "{}";
      views<char_t, nb_args + 1> fmt_parts = split_n<nb_args + 1>(fmt, sep);

      sizes<nb_args + 1> fs = fmt_sizes(fmt_parts);
      sizes<nb_args> ts = {length_of(args)...};

      std::size_t i(0);
      std::size_t offset(0);
      formatter_context<char_t> subctx;

      ((
           subctx.data = ctx.data + offset, subctx.len = fs[i],
           format_of(subctx, fmt_parts.at(i)),
           offset += fs[i],
           subctx.data = ctx.data + offset, subctx.len = ts[i],
           format_of(subctx, args),
           offset += ts[i],
           i++),
       ...);

      subctx.data = ctx.data + offset;
      subctx.len = fs[i];
      format_of(subctx, fmt_parts[i]);
    }
  } // namespace detail

  template <typename... args_t>
  buffer<char> format(
      std::basic_string_view<char> fmt,
      const args_t &...args)
  {
    return detail::format(fmt, args...);
  }

  template <typename... args_t>
  buffer<wchar_t> format(
      std::basic_string_view<wchar_t> fmt,
      const args_t &...args)
  {
    return detail::format(fmt, args...);
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
