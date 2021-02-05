
#include <iostream>
#include "format.hpp"

struct person
{
  int age;
  std::string prenom;
};

std::size_t length_of(const person &p)
{
  std::size_t len = std::basic_string_view<char>(" et j'ai ").size();

  return clon::fmt::length_of(std::basic_string_view<char>(p.prenom)) + len + clon::fmt::length_of(p.age);
}

template <typename char_t>
void format_of(
    clon::fmt::formatter_context<char_t> &ctx,
    const person &p)
{
  auto &&prenom = std::basic_string_view<char_t>(p.prenom);
  auto &&etjai = std::basic_string_view<char_t>(" et j'ai ");

  clon::fmt::formatter_context<char_t> subctx;

  subctx = ctx.subcontext(0, prenom.size());
  clon::fmt::format_of(subctx, prenom);
  subctx = subctx.subcontext(subctx.len, etjai.size());
  clon::fmt::format_of(subctx, etjai);
  subctx = subctx.subcontext(subctx.len, clon::fmt::length_of(p.age));
  clon::fmt::format_of(subctx, p.age);
}

int main()
{
  person Bob{12, "Bob"};
  std::cout << clon::fmt::format("je suis {}\n", Bob);

  //std::cout << format<std::string>("{}{}{} \n", str("coucou"), idec(10302890), bl(false));
  //std::cout << format<std::string>(str("coucou"), idec(10302890), bl(false), str("\n"));

  // std::string s("coucou");
  // std::cout << format<std::string>("{}-{}", str(s), cstr("\n"));
  // std::cout << format<std::string>("{}-{}--{}---{}---", idec(1302), bl(false), cstr("true"), cstr("\n"));
}
