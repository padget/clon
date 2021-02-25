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
    const type_t &as_() const
    {
      return std::get<type_t>(root->nodes[index].val);
    }

    template <typename type_t>
    const bool is_() const
    {
      if constexpr (std::is_same_v<type_t, boolean>)
        return type() == clon_type::boolean;
      if constexpr (std::is_same_v<type_t, list>)
        return type() == clon_type::list;
      if constexpr (std::is_same_v<type_t, string<char_t>>)
        return type() == clon_type::string;
      if constexpr (std::is_same_v<type_t, number>)
        return type() == clon_type::number;
      if constexpr (std::is_same_v<type_t, none>)
        return type() == clon_type::none;
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
  struct childs_iterator
  {
    root_view<char_t> view;

    childs_iterator &operator++()
    {
      view.index = view.next();
      return *this;
    }

    childs_iterator operator++(int)
    {
      childs_iterator tmp(*this);
      ++(*this);
      return tmp;
    }

    root_view<char_t> operator*() const { return make_rview(view, view.index); }

    friend bool operator==(
        const childs_iterator &a,
        const childs_iterator &b)
    {
      return a.view.root == b.view.root and
             a.view.index == b.view.index;
    }

    friend bool operator!=(
        const childs_iterator &a,
        const childs_iterator &b)
    {
      return not(a == b);
    }
  };

  template <typename char_t>
  struct childs_list
  {
    root_view<char_t> view;
    childs_iterator<char_t> begin() const { return {view}; }
    childs_iterator<char_t> end() const { return {make_rview(view, no_next)}; }
  };

  template <typename char_t>
  childs_list<char_t> childs(const root_view<char_t> &view)
  {
    return {make_rview(view, view.child())};
  }

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
    namespace fmt = clon::fmt;

    switch (view.type())
    {
    case clon_type::boolean:
    case clon_type::number:
    case clon_type::string:
      fmt::format_into(ctx, "({} {})", view.name(), view.valv());
      break;
    case clon_type::list:
      fmt::format_into(ctx, "({} ", view.name());
      for (auto &&child : childs(view))
        format_of(ctx, child);
      fmt::format_into(ctx, ")");
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
  struct scanner
  {
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

    bool starts_with(const std::basic_string_view<char_t> &sv) const
    {
      return data.substr(index).starts_with(sv);
    }
  };

  template <typename char_t, std::size_t n>
  void handle_error_expecting(const char_t (&s)[n])
  {
    throw std::runtime_error(clon::fmt::format("expected character {}", s));
  }

  template <typename char_t>
  void ignore_blanks(scanner<char_t> &scan)
  {
    while (scan.symbol() == symbol_type::blank)
      scan.advance();

    scan.ignore();
  }

  template <typename char_t>
  std::basic_string_view<char_t> scan_name(
      scanner<char_t> &scan)
  {
    ignore_blanks(scan);

    symbol_type sb = scan.symbol();

    if (sb != symbol_type::lower and
        sb != symbol_type::letter_f and
        sb != symbol_type::letter_t)
      handle_error_expecting("[a-z]");

    while (sb == symbol_type::lower or
           sb == symbol_type::letter_f or
           sb == symbol_type::letter_t)
    {
      scan.advance();
      sb = scan.symbol();
    }

    return scan.extract();
  }

  template <typename char_t>
  std::basic_string_view<char_t> scan_boolean(
      scanner<char_t> &scan)
  {
    ignore_blanks(scan);

    if (scan.starts_with("true"))
      scan.advance(4);
    else if (scan.starts_with("false"))
      scan.advance(5);
    else
      handle_error_expecting("'true' or 'false'");

    return scan.extract();
  }

  template <typename char_t>
  std::basic_string_view<char_t> scan_string(
      scanner<char_t> &scan)
  {
    ignore_blanks(scan);

    if (scan.symbol() == symbol_type::dquote)
    {
      scan.advance();

      while (scan.symbol() != symbol_type::dquote)
        scan.advance();

      if (scan.symbol() == symbol_type::dquote)
        scan.advance();
      else
        handle_error_expecting("'\"'");
    }
    else
      handle_error_expecting("'\"'");

    return scan.extract();
  }

  template <typename char_t>
  std::basic_string_view<char_t> scan_number(
      scanner<char_t> &scan)
  {
    ignore_blanks(scan);

    if (scan.symbol() != symbol_type::digit)
      handle_error_expecting("[0-9]");

    while (scan.symbol() == symbol_type::digit)
      scan.advance();

    return scan.extract();
  }

  template <typename char_t>
  std::basic_string_view<char_t> scan_list(
      scanner<char_t> &)
  {
    return {};
  }

  template <typename char_t>
  void open_node(scanner<char_t> &scan)
  {
    ignore_blanks(scan);

    if (scan.symbol() == symbol_type::lpar)
    {
      scan.advance();
      scan.ignore();
    }
    else
      handle_error_expecting("'('");
  }

  template <typename char_t>
  void close_node(scanner<char_t> &scan)
  {
    ignore_blanks(scan);

    if (scan.symbol() == symbol_type::rpar)
    {
      scan.advance();
      scan.ignore();
    }
    else
      handle_error_expecting("')'");
  }

  template <typename char_t>
  struct parser_context
  {
    std::vector<node<char_t>> *nodes;
    scanner<char_t> scan;
  };

  template <typename char_t>
  void parse_node(parser_context<char_t> &ctx);

  template <typename char_t>
  void parse_list(parser_context<char_t> &ctx)
  {
    ignore_blanks(ctx.scan);
    std::size_t index = ctx.nodes->size();
    std::size_t count = 0;

    while (ctx.scan.symbol() == symbol_type::lpar)
    {
      if (count > 0)
        index = ctx.nodes->at(index).next = ctx.nodes->size();

      parse_node(ctx);
      ignore_blanks(ctx.scan);
      count++;
    }
  }

  template <typename char_t>
  clon_type predict_clon_type(scanner<char_t> &scan)
  {
    switch (scan.symbol())
    {
    case symbol_type::letter_f:
    case symbol_type::letter_t:
      return clon_type::boolean;
    case symbol_type::dquote:
      return clon_type::string;
    case symbol_type::digit:
      return clon_type::number;
    case symbol_type::lpar:
      return clon_type::list;
    default:
      return clon_type::none;
    }
  }

  template <typename char_t>
  void parse_node(parser_context<char_t> &ctx)
  {
    open_node(ctx.scan);

    std::basic_string_view<char_t> &&name(scan_name(ctx.scan));
    ignore_blanks(ctx.scan);

    std::basic_string_view<char_t> scanr;
    clon_type type = predict_clon_type(ctx.scan);

    switch (type)
    {
    case clon_type::boolean:
      scanr = scan_boolean(ctx.scan);
      break;
    case clon_type::string:
      scanr = scan_string(ctx.scan);
      break;
    case clon_type::number:
      scanr = scan_number(ctx.scan);
      break;
    case clon_type::list:
      scanr = scan_list(ctx.scan);
      break;
    case clon_type::none:
      break;
    }

    ctx.nodes->emplace_back(make_node(type, name, scanr));

    if (type == clon_type::list)
    {
      ctx.nodes->back().child = ctx.nodes->size();
      parse_list(ctx);
    }

    close_node(ctx.scan);
  }

  template <typename char_t>
  const root_node<char_t> parse(
      const std::basic_string_view<char_t> &data)
  {
    root_node<char_t> &&root = make_root(data);
    parser_context<char_t> ctx{&root.nodes, {data}};
    parse_node(ctx);
    return root;
  }

  constexpr std::size_t path_max = maxof<std::size_t>;

  template <typename char_t>
  struct path
  {
    std::basic_string_view<char_t> name;
    std::size_t min = path_max;
    std::size_t max = path_max;
  };

  template <typename char_t>
  bool scan_colon(scanner<char_t> &scan)
  {
    if (scan.data[scan.index] == ':')
    {
      scan.advance();
      scan.ignore();
      return true;
    }

    return false;
  }

  template <typename char_t>
  std::pair<std::size_t, std::size_t> scan_interval(scanner<char_t> &scan)
  {
    if (scan.data[scan.index] == '*')
    {
      scan.ignore();
      return {path_max, path_max};
    }
    else
    {
      std::size_t &&min = to_integer(scan_number(scan));
      return {min, min};
    }
  }

  template <typename char_t>
  path<char_t> parse_path(const std::basic_string_view<char_t> &view)
  {
    path<char_t> p;
    scanner<char_t> scan(view);
    p.name = scan_name(scan);

    if (scan_colon(scan))
      std::tie(p.min, p.max) = scan_interval(scan);
    else
      p.min = p.max = 0;

    return p;
  }

  template <typename char_t>
  struct splits_iterator
  {
    char_t del;
    std::basic_string_view<char_t> data;

    splits_iterator &operator++()
    {
      if (data.size() != 0)
      {
        auto found = std::find(data.begin(), data.end(), del);
        data = std::basic_string_view<char_t>(
            found == data.end() ? found : found + 1, data.end());
      }

      return *this;
    }

    const std::basic_string_view<char_t> operator*()
    {
      return std::basic_string_view<char_t>(
          data.begin(), std::find(data.begin(), data.end(), del));
    }

    splits_iterator operator++(int)
    {
      splits_iterator tmp(*this);
      ++(*this);
      return tmp;
    }

    friend bool operator==(
        const splits_iterator &a,
        const splits_iterator &b)
    {
      return a.data.data() == b.data.data();
    }

    friend bool operator!=(
        const splits_iterator &a,
        const splits_iterator &b)
    {
      return not(a == b);
    }
  };

  template <typename char_t>
  struct splits
  {
    char_t del;
    std::basic_string_view<char_t> data;

    splits_iterator<char_t> begin() const
    {
      return splits_iterator<char_t>{del, data};
    }

    splits_iterator<char_t> end() const
    {
      return splits_iterator<char_t>{del, data.substr(data.size())};
    }
  };

  template <typename char_t>
  splits<char_t> split(
      const std::basic_string_view<char_t> &data,
      const char_t &del)
  {
    return splits<char_t>{del, data};
  }

  template <typename char_t>
  struct paths_iterator
  {
    splits_iterator<char_t> sit;

    paths_iterator &operator++()
    {
      ++sit;
      return *this;
    }

    paths_iterator operator++(int)
    {
      paths_iterator tmp(*this);
      ++(*this);
      return tmp;
    }

    path<char_t> operator*()
    {
      return parse_path(*sit);
    }

    friend bool operator==(
        const paths_iterator &a,
        const paths_iterator &b)
    {
      return a.sit == b.sit;
    }

    friend bool operator!=(
        const paths_iterator &a,
        const paths_iterator &b)
    {
      return not(a == b);
    }
  };

  template <typename char_t>
  struct paths
  {
    splits<char_t> spl;

    paths_iterator<char_t> begin() const { return {spl.begin()}; }
    paths_iterator<char_t> end() const { return {spl.end()}; }
  };

  template <typename char_t>
  paths<char_t> split_paths(const std::basic_string_view<char_t> &pths)
  {
    return paths<char_t>{split(pths, '.')};
  }

  template <typename char_t>
  root_view<char_t> getone(
      const path<char_t> &pth,
      const root_view<char_t> &view)
  {
    std::size_t cnt = 0;

    if (view.type() == clon_type::list)
      for (root_view<char_t> &&child : childs(view))
        if (child.name() == pth.name)
        {
          if (cnt == pth.min)
            return child;
          else
            ++cnt;
        }

    return make_rview(view, no_root);
  }

  template <typename char_t>
  root_view<char_t> getone(
      const std::basic_string_view<char_t> &pth,
      const root_view<char_t> &view)
  {
    return getone(parse_path(pth), view);
  }

  template <typename char_t>
  root_view<char_t> get(
      const std::basic_string_view<char_t> &pths,
      const root_view<char_t> &view)
  {
    bool should_return = false;

    root_view<char_t> vfound = view;

    for (const path<char_t> &pth : split_paths(pths))
    {
      bool found = false;

      if (should_return)
        return make_rview(view, no_root);

      if (vfound.template is_<list>())
      {
        std::size_t cnt = 0;

        for (root_view<char_t> &&child : childs(vfound))
          if (child.name() == pth.name)
          {
            if (cnt == pth.min)
            {
              found = true;
              vfound = child;
              break;
            }
            else
              ++cnt;
          }
      }
      else
        should_return = true;

      if (not found)
        return make_rview(view, no_root);
    }

    return vfound;
  }

} // namespace clon

namespace clon
{
  template <typename char_t>
  class basic_api_view;

  template <typename char_t>
  class basic_api : public basic_api_view<char_t>
  {
    root_node<char_t> node;

  public:
    explicit basic_api(const std::basic_string_view<char_t> &_v)
        : basic_api_view<char_t>(make_rview(node)), node(parse(_v)) {}
  };

  template <typename char_t>
  class basic_api_view
  {
    root_view<char_t> view;

  public:
    explicit basic_api_view(const root_view<char_t> &_v)
        : view(_v) {}

  public:
    basic_api_view<char_t> get(const std::basic_string_view<char_t> &pth) const
    {
      return basic_api_view<char_t>(clon::get(pth, view));
    }

    basic_api_view<char_t> operator[](const std::basic_string_view<char_t> &pth) const
    {
      return get(pth);
    }

    const root_view<char_t> &v() const { return view; }
    clon_type type() const { return view.type(); }
    const std::basic_string_view<char_t> &name() const { return view.name(); }
  };

  template <typename char_t>
  std::size_t length_of(const basic_api_view<char_t> &a)
  {
    return length_of(a.v());
  }

  template <typename char_t>
  void format_of(
      clon::fmt::formatter_context<char_t> &ctx,
      const basic_api_view<char_t> &a)
  {
    format_of(ctx, a.v());
  }

  using api = basic_api<char>;
  using wapi = basic_api<wchar_t>;
}
#endif