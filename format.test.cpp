
#include <iosfwd>
#include "format.hpp"
#include <iostream>
#include <array>
#include <string_view>
#include <vector>

int main()
{
  namespace fmt = clon::fmt;
  using namespace fmt;

  auto &&src = init_cspan("cou{}hqlskdj{}ddds{}qsldj{}qsldj{}");
  auto &&sep = init_cspan("{}");
  for (auto &&item : split_n<12>(src, sep))
  {
    std::cout << '"';
    for (auto &&c : item)
    {
      std::cout << c;
    }
    std::cout << '"';
    std::cout << '\n';
  }
  //std::cout << format<std::string>("{}{}{} \n", str("coucou"), idec(10302890), bl(false));
  //std::cout << format<std::string>(str("coucou"), idec(10302890), bl(false), str("\n"));

  // std::string s("coucou");
  // std::cout << format<std::string>("{}-{}", str(s), cstr("\n"));
  // std::cout << format<std::string>("{}-{}--{}---{}---", idec(1302), bl(false), cstr("true"), cstr("\n"));
}
