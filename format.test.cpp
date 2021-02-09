
#include <iostream>
#include "format.hpp"

namespace std
{
  template <typename char_t>
  std::size_t length_of(const std::basic_string<char_t> &s)
  {
    return s.size();
  }

  template <typename char_t>
  void format_of(clon::fmt::formatter_context<char_t> &ctx, const std::string &s)
  {
    clon::fmt::format_of(ctx, std::basic_string_view<char_t>(s));
  }
} // namespace std

struct person
{
  int age;
  std::string prenom;
};

std::size_t length_of(const person &p)
{
  std::cout << "length of personn\n";
  return clon::fmt::predict_length_of("{} et j'ai {} ans", p.prenom, p.age);
}

template <typename char_t>
void format_of(
    clon::fmt::formatter_context<char_t> &ctx,
    const person &p)
{
  clon::fmt::format_into(ctx, "{} et j'ai {} ans", p.prenom, p.age);
}

struct maison
{
  person p;
};

std::size_t length_of(const maison &m)
{
  return clon::fmt::predict_length_of("{}", m.p);
}

template <typename char_t>
void format_of(
    clon::fmt::formatter_context<char_t> &ctx,
    const maison &m)
{
  clon::fmt::format_into(ctx, "{}", m.p);
}


int main()
{
  person Bob{12, "Bob"};
  maison m(Bob);
  std::cout << clon::fmt::format("{}\n", m);

  //std::cout << format<std::string>("{}{}{} \n", str("coucou"), idec(10302890), bl(false));
  //std::cout << format<std::string>(str("coucou"), idec(10302890), bl(false), str("\n"));

  // std::string s("coucou");
  // std::cout << format<std::string>("{}-{}", str(s), cstr("\n"));
  // std::cout << format<std::string>("{}-{}--{}---{}---", idec(1302), bl(false), cstr("true"), cstr("\n"));
}
