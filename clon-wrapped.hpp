#ifndef __clon_clon_wrapped_hpp__
#define __clon_clon_wrapped_hpp__

#include <string_view>
#include <variant>
#include <vector>
#include <span>

namespace clon
{
  template<typename char_t>
  using sview = std::basic_string_view<char_t>;

  enum struct clon_type : unsigned
  {
    none = 0,
    boolean = 1,
    number = 2,
    string = 3,
    object = 4
  };

  template <typename char_t>
  struct node;

  template <typename char_t, typename type_t>
  struct wrapper
  {
    sview<char_t> valv;
  };

  template <typename char_t>
  using number = wrapper<char_t, double>;

  template <typename char_t>
  using string = wrapper<char_t, sview<char_t>>;

  template <typename char_t>
  using boolean = wrapper<char_t, bool>;

  template <typename char_t>
  using object = wrapper<char_t, std::monostate>;

  using none = std::monostate;

  template <typename char_t>
  using value = std::variant<
      none,
      boolean<char_t>,
      number<char_t>,
      string<char_t>,
      object<char_t>>;

  template<typename char_t, typename type_t>
  concept possible_value = 
  // TODO std::same_as<type_t, none> or
  std::same_as<type_t, boolean<char_t>> or
  std::same_as<type_t, number<char_t>> or
  std::same_as<type_t, string<char_t>> or
  std::same_as<type_t, object<char_t>> ;
  
  template <typename char_t>
  class node
  {
  public:
  template<possible_value<char_t> value_t>
    explicit node(
        sview<char_t> _name,
        const value_t& _val,
        std::size_t _next,
        std::size_t _parent)
        : name(_name),
          val(_val),
          next(_next),
          parent(_parent) {}

  public:
    const clon_type type() const noexcept
    {
      return static_cast<clon_type>(val.index());
    }

    template <clon::constraint::possible_value type_t>
    const bool is_() const
    {
      if constexpr (std::is_same_v<type_t, boolean<char_t>>)
        return type(c) == clon_type::boolean;
      if constexpr (std::is_same_v<type_t, object<char_t>>)
        return type(c) == clon_type::object;
      if constexpr (std::is_same_v<type_t, string<char_t>>)
        return type(c) == clon_type::string;
      if constexpr (std::is_same_v<type_t, number<char_t>>)
        return type(c) == clon_type::number;
      if constexpr (std::is_same_v<type_t, none>)
        return type(c) == clon_type::none;
    }

    template <clon::constraint::possible_value type_t>
    const type_t &as_() const
    {
      return std::get<type_t>(val);
    }

  private:
    sview<char_t> name;
    value<char_t> val;
    std::size_t next;
    std::size_t parent;
  };

  class default_parser
  {
  public:
    template <typename char_t>
    void parse(
        const std::vector<char_t> &buff,
        std::vector<node<char_t>> &vals) const noexcept
    {
    }
  };

  template <typename char_t,
            typename parser_t = default_parser>
  class root final
  {
  public:
    explicit root(sview<char_t> v) noexcept
        : buff(v.begin(), v.end())
    {
      vals.reserve(std::count(buff.begin(), buff.end(), '('));
      parser_t{}.parse(buff, vals);
    }

  public:
    static const node<char_t> &undefined() noexcept
    {
      static node<char_t> undef;
      return undef;
    }

    const sview<char_t> to_string() const noexcept
    {
      std::string s;
      s.reserve(buff.size());
      return s;
    }

    const node &get(sview<char_t> pth) const noexcept
    {
      return undefined();
    }

    const node &operator[](sview<char_t> pth) const noexcept
    {
      return get(pth);
    }

    bool parsed() const noexcept
    {
      return not vals.empty();
    }

  private:


  private:
    std::vector<char_t> buff;
    std::vector<node<char_t>> vals;
  };

} // namespace clon

#endif