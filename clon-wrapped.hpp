#ifndef __clon_clon_wrapped_hpp__
#define __clon_clon_wrapped_hpp__

#include <string_view>
#include <string>
#include <variant>
#include <vector>
#include <limits>
#include <array>
#include <stdexcept>

#include "format.hpp"

namespace clon
{
  enum struct clon_type : unsigned
  {
    none = 0,
    boolean = 1,
    number = 2,
    string = 3,
    list = 4
  };

  struct list_tag
  {
  };

  template <typename char_t>
  using string = std::basic_string_view<char_t>;
  using none = std::monostate;
  using number = double;
  using boolean = bool;
  using list = list_tag;

  template <typename char_t>
  using value = std::variant<none, boolean, number, string<char_t>, list>;

  template <typename char_t, typename type_t>
  concept possible_value =
      std::same_as<type_t, boolean> or
      std::same_as<type_t, number> or
      std::same_as<type_t, string<char_t>> or
      std::same_as<type_t, list>;

  template <std::integral t>
  constexpr t maxof = std::numeric_limits<t>::max();

  constexpr std::size_t no_next = maxof<std::size_t>;
  constexpr std::size_t no_child = maxof<std::size_t>;
  constexpr std::size_t no_root = maxof<std::size_t>;

  template <typename char_t>
  number to_number(std::basic_string_view<char_t> v)
  {
    number n = 0;

    for (const char_t &c : v)
      n = n * 10 + (c - '0');

    return n;
  }

  template <typename char_t>
  std::size_t to_integer(std::basic_string_view<char_t> v)
  {
    std::size_t n = 0;

    for (const char_t &c : v)
      n = n * 10 + (c - '0');

    return n;
  }

  template <typename char_t>
  struct node
  {
    std::basic_string_view<char_t> name;
    value<char_t> val;
    std::basic_string_view<char_t> valv;
    std::size_t next = no_next;
    std::size_t child = no_child;
  };

  template <typename char_t>
  node<char_t> make_node(
      const clon_type &type,
      const std::basic_string_view<char_t> &name,
      const std::basic_string_view<char_t> &valv)
  {
    node<char_t> n;

    n.name = name;
    n.valv = valv;

    if (type == clon_type::boolean)
      n.val = valv == "true";
    else if (type == clon_type::number)
      n.val = to_number(valv);
    else if (type == clon_type::string)
      n.val = valv;
    else if (type == clon_type::list)
      n.val = list();
    else if (type == clon_type::none)
      n.val = std::monostate();

    return n;
  }

  template <typename char_t>
  struct root_node
  {
    std::vector<char_t> buff;
    std::vector<node<char_t>> nodes;
  };

  template <typename char_t>
  root_node<char_t> make_root(
      const std::basic_string_view<char_t> &data)
  {
    root_node<char_t> root;
    root.buff = std::vector<char_t>(data.begin(), data.end());
    root.nodes.reserve(std::count(data.begin(), data.end(), '('));
    return root;
  }

  template <typename char_t>
  struct root_view
  {
    const root_node<char_t> *root;
    std::size_t index;

    std::basic_string<char_t> to_string() const
    {
      return clon::fmt::format("{}", *this);
    }

    using view = std::basic_string_view<char_t>;

    const view &name() const { return root->nodes[index].name; }
    const std::size_t &child() const { return root->nodes[index].child; }
    const std::size_t &next() const { return root->nodes[index].next; }
    const view &valv() const { return root->nodes[index].valv; }
    const clon_type type() const { return static_cast<clon_type>(root->nodes[index].val.index()); }

    template <typename type_t>
    requires possible_value<char_t, type_t> const type_t &as_() const
    {
      return std::get<type_t>(root->nodes[index].val);
    }

    template <typename type_t>
    requires possible_value<char_t, type_t> const bool is_() const
    {
      using cl = clon_type;

      if constexpr (std::is_same_v<type_t, boolean>)
        return type() == cl::boolean;
      if constexpr (std::is_same_v<type_t, list>)
        return type() == cl::list;
      if constexpr (std::is_same_v<type_t, string<char_t>>)
        return type() == cl::string;
      if constexpr (std::is_same_v<type_t, number>)
        return type() == cl::number;
      if constexpr (std::is_same_v<type_t, none>)
        return type() == cl::none;
    }
  };

  template <typename char_t>
  root_view<char_t> make_rview(
      const root_node<char_t> &r,
      const std::size_t &index = 0)
  {
    return {&r, index < r.nodes.size() ? index : no_root};
  }

  template <typename char_t>
  root_view<char_t> make_rview(
      const root_view<char_t> &view,
      const std::size_t &index = 0)
  {
    return make_rview(*view.root, index);
  }

  template <typename char_t>
  struct nexts_iterator
  {
    root_view<char_t> view;

    nexts_iterator &operator++()
    {
      view.index = view.next();
      return *this;
    }

    nexts_iterator operator++(int)
    {
      nexts_iterator tmp(*this);
      ++(*this);
      return tmp;
    }

    const std::size_t &operator*() { return view.index; }

    friend bool operator==(
        const nexts_iterator &a,
        const nexts_iterator &b)
    {
      return a.view.root == b.view.root and
             a.view.index == b.view.index;
    }

    friend bool operator!=(
        const nexts_iterator &a,
        const nexts_iterator &b)
    {
      return not(a == b);
    }
  };

  template <typename char_t>
  struct nexts
  {
    root_view<char_t> view;

    nexts_iterator<char_t> begin() const { return {view}; }
    nexts_iterator<char_t> end() const { return {make_rview(view, no_root)}; }
  };

  template <typename char_t>
  nexts<char_t> indexes(const root_view<char_t> view) { return {view}; }

  template <typename char_t>
  std::size_t length_of(const root_view<char_t> &view)
  {
    return view.root->buff.size();
  }

  template <typename char_t>
  void format_of(
      clon::fmt::formatter_context<char_t> &ctx,
      const root_view<char_t> &view)
  {
    switch (view.type())
    {
    case clon_type::boolean:
    case clon_type::number:
    case clon_type::string:
      clon::fmt::format_into(ctx, "({} {})", view.name(), view.valv());
      break;
    case clon_type::list:
      clon::fmt::format_into(ctx, "({} ", view.name());
      for (auto &&index : indexes(make_rview(view, view.child())))
        format_of(ctx, make_rview(view, index));
      clon::fmt::format_into(ctx, ")");
      break;
    case clon_type::none:
      break;
    }
  }

  enum class symbol_type : int
  {
    eos,
    blank,
    letter_f,
    letter_t,
    lower,
    dquote,
    digit,
    lpar,
    rpar,
    other
  };

  constexpr std::array<symbol_type, 128> ascii_build()
  {
    std::array<symbol_type, 128> table;

    for (symbol_type &sb : table)
      sb = symbol_type::other;

    for (char l = 'a'; l <= 'z'; ++l)
      table[l] = symbol_type::lower;

    for (char l = '0'; l <= '9'; ++l)
      table[l] = symbol_type::digit;

    table['f'] = symbol_type::letter_f;
    table['t'] = symbol_type::letter_t;
    table['"'] = symbol_type::dquote;
    table['('] = symbol_type::lpar;
    table[')'] = symbol_type::rpar;
    table[' '] = symbol_type::blank;
    table['\n'] = symbol_type::blank;
    table['\r'] = symbol_type::blank;
    table['\t'] = symbol_type::blank;
    table['\0'] = symbol_type::eos;

    return table;
  }

  constexpr std::array<symbol_type, 128> ascii_to_sb = ascii_build();

  template <typename char_t>
  struct parser_context
  {
    std::vector<node<char_t>> *nodes;
    std::basic_string_view<char_t> data;
    std::size_t index = 0;
    std::size_t prev = 0;

    symbol_type symbol() const
    {
      if (index < data.size())
        return data[index] > 127
                   ? symbol_type::other
                   : ascii_to_sb[data[index]];
      else
        return symbol_type::eos;
    }

    bool starts_with(const std::basic_string_view<char_t> &sv)
    {
      return data.substr(index).starts_with(sv);
    }

    void advance(std::size_t step = 1)
    {
      if (index + step <= data.size())
        index += step;
    }

    std::basic_string_view<char_t> extract()
    {
      std::basic_string_view<char_t> tk(&data[prev], index - prev);
      prev = index;
      return tk;
    }

    void ignore()
    {
      prev = index;
    }
  };

  template <typename char_t, std::size_t n>
  void handle_error_expecting(const char_t (&s)[n])
  {
    throw std::runtime_error(clon::fmt::format("expected character {}", s));
  }

  template <typename char_t>
  void ignore_blanks(parser_context<char_t> &ctx)
  {
    while (ctx.symbol() == symbol_type::blank)
      ctx.advance();

    ctx.ignore();
  }

  template <typename char_t>
  void open_node(parser_context<char_t> &ctx)
  {
    ignore_blanks(ctx);

    if (ctx.symbol() == symbol_type::lpar)
    {
      ctx.advance();
      ctx.ignore();
    }
    else
      handle_error_expecting("'('");
  }

  template <typename char_t>
  std::basic_string_view<char_t> scan_name(
      parser_context<char_t> &ctx)
  {
    ignore_blanks(ctx);

    symbol_type sb = ctx.symbol();

    if (sb != symbol_type::lower and
        sb != symbol_type::letter_f and
        sb != symbol_type::letter_t)
      handle_error_expecting("[a-z]");

    while (sb == symbol_type::lower or
           sb == symbol_type::letter_f or
           sb == symbol_type::letter_t)
    {
      ctx.advance();
      sb = ctx.symbol();
    }

    return ctx.extract();
  }

  template <typename char_t>
  std::basic_string_view<char_t> scan_boolean(
      parser_context<char_t> &ctx)
  {
    ignore_blanks(ctx);

    if (ctx.starts_with("true"))
      ctx.advance(4);
    else if (ctx.starts_with("false"))
      ctx.advance(5);
    else
      handle_error_expecting("'true' or 'false'");

    return ctx.extract();
  }

  template <typename char_t>
  std::basic_string_view<char_t> scan_string(
      parser_context<char_t> &ctx)
  {
    ignore_blanks(ctx);

    if (ctx.symbol() == symbol_type::dquote)
    {
      ctx.advance();

      while (ctx.symbol() != symbol_type::dquote)
        ctx.advance();

      if (ctx.symbol() == symbol_type::dquote)
        ctx.advance();
      else
        handle_error_expecting("'\"'");
    }
    else
      handle_error_expecting("'\"'");

    return ctx.extract();
  }

  template <typename char_t>
  std::basic_string_view<char_t> scan_number(
      parser_context<char_t> &ctx)
  {
    ignore_blanks(ctx);

    if (ctx.symbol() != symbol_type::digit)
      handle_error_expecting("[0-9]");

    while (ctx.symbol() == symbol_type::digit)
      ctx.advance();

    return ctx.extract();
  }

  template <typename char_t>
  std::basic_string_view<char_t> scan_list(
      parser_context<char_t> &)
  {
    return {};
  }

  template <typename char_t>
  void close_node(parser_context<char_t> &ctx)
  {
    ignore_blanks(ctx);

    if (ctx.symbol() == symbol_type::rpar)
    {
      ctx.advance();
      ctx.ignore();
    }
    else
      handle_error_expecting("')'");
  }

  template <typename char_t>
  void parse_node(parser_context<char_t> &ctx);

  template <typename char_t>
  void parse_list(parser_context<char_t> &ctx)
  {
    ignore_blanks(ctx);
    std::size_t index = ctx.nodes->size();
    std::size_t count = 0;

    while (ctx.symbol() == symbol_type::lpar)
    {
      if (count > 0)
        index = ctx.nodes->at(index).next = ctx.nodes->size();

      parse_node(ctx);
      ignore_blanks(ctx);
      count++;
    }
  }

  template <typename char_t>
  void parse_node(parser_context<char_t> &ctx)
  {
    open_node(ctx);

    std::basic_string_view<char_t> &&name(scan_name(ctx));
    ignore_blanks(ctx);

    switch (ctx.symbol())
    {
    case symbol_type::letter_f:
    case symbol_type::letter_t:
      ctx.nodes->emplace_back(make_node(clon_type::boolean, name, scan_boolean(ctx)));
      break;
    case symbol_type::dquote:
      ctx.nodes->emplace_back(make_node(clon_type::string, name, scan_string(ctx)));
      break;
    case symbol_type::digit:
      ctx.nodes->emplace_back(make_node(clon_type::number, name, scan_number(ctx)));
      break;
    case symbol_type::lpar:
      ctx.nodes->emplace_back(make_node(clon_type::list, name, scan_list(ctx)));
      ctx.nodes->back().child = ctx.nodes->size();
      parse_list(ctx);
      break;
    default:
      break;
    }

    close_node(ctx);
  }

  template <typename char_t>
  const root_node<char_t> parse(
      const std::basic_string_view<char_t> &data)
  {
    root_node<char_t> &&root = make_root(data);
    parser_context<char_t> ctx{&root.nodes, data};
    parse_node(ctx);s
    return root;
  }

  // constexpr std::size_t path_max = maxof<std::size_t>;

  // template <typename char_t>
  // struct path
  // {
  //   std::basic_string_view<char_t> name;
  //   std::size_t min;
  //   std::size_t max;
  // };

  // template <typename char_t>
  // class path_parser
  // {
  // public:
  //   explicit path_parser(std::basic_string_view<char_t> _path)
  //       : __data(_path) {}

  // public:
  //   path<char_t> parse()
  //   {
  //     path<char_t> p;
  //     p.name = scan_name();

  //     if (scan_colon())
  //     {
  //       auto &&[min, max] = scan_interval();

  //       p.max = max;
  //       p.min = min;
  //     }

  //     return p;
  //   }

  //   bool scan_colon()
  //   {
  //     if (__data[__index] == ':')
  //     {
  //       advance();
  //       ignore();
  //       return true;
  //     }

  //     return false;
  //   }

  //   std::pair<std::size_t, std::size_t> scan_interval()
  //   {
  //     if (__data[__index] == '*')
  //       return {path_max, path_max};
  //     else
  //     {
  //       std::size_t &&min = to_integer(scan_number());
  //       return {min, min};
  //     }
  //   }

  //   std::basic_string_view<char_t> scan_name()
  //   {
  //     if (not('a' <= __data[__index] and __data[__index] <= 'z'))
  //       handle_error_expecting("[a-z]");

  //     while ('a' <= __data[__index] and __data[__index] <= 'z')
  //       advance();

  //     return extract();
  //   }

  //   std::basic_string_view<char_t> scan_number()
  //   {
  //     if (not('0' <= __data[__index] and __data[__index] <= '9'))
  //       handle_error_expecting("[0-9]");

  //     while ('0' <= __data[__index] and __data[__index] <= '9')
  //       advance();

  //     return extract();
  //   }

  // private:
  //   void advance(std::size_t step = 1)
  //   {
  //     if (__index + step <= __data.size())
  //       __index += step;
  //   }

  //   std::basic_string_view<char_t> extract()
  //   {
  //     std::basic_string_view<char_t> tk(&__data[__prev], __index - __prev);
  //     __prev = __index;
  //     return tk;
  //   }

  //   void ignore()
  //   {
  //     __prev = __index;
  //   }

  //   template <std::size_t n>
  //   void handle_error_expecting(const char (&expected)[n])
  //   {
  //     throw std::runtime_error(
  //         clon::fmt::format(
  //             "scanning : expected {} at index {}", expected, __index));
  //   }

  // private:
  //   std::basic_string_view<char_t> __data;
  //   std::size_t __index = 0;
  //   std::size_t __prev = 0;
  // };

  // template <typename char_t>
  // root_view<char_t> explore(
  //     const path<char> &pth,
  //     const root_view<char_t> &r)
  // {
  //   std::size_t cnt = 0;

  //   for (auto &&ni : r.child())
  //   {
  //     std::cout << fmt::format("{}\n", ni.first.name());
  //     if (ni.first.name() == pth.name)
  //     {
  //       if (cnt == pth.min and cnt == pth.max)
  //         return r.view(ni.second);
  //       else
  //         ++cnt;
  //     }
  //   }
  //   return r.view(no_root);
  // }

  // template <typename char_t>
  // root_view<char_t> get(
  //     const path<char> &pth,
  //     const root<char_t> &r)
  // {
  //   return explore(pth, r.view());
  // }

} // namespace clon

#endif