
#include <iosfwd>
#include "format.hpp"
#include <iostream>

#include <string_view>
#include <vector>

int main()
{
  namespace fmt = clon::fmt;
  using namespace fmt;

  pattern<const char, 5> p(
      chars_span<const char>("cou{}hqlskdj{}ddds{}qsldj{}qsldj"),
      idec(1302), bl(false), cstr("true"), cstr("true"), cstr("true"));
//"cou1302hqlskdjfalsedddstrueqsldjtrue qsldj"
  for (int i = 0; i < 5; i++)
    std::cout << p.coords().data[i] << std::endl;

  //std::cout << format<std::string>("{}{}{} \n", str("coucou"), idec(10302890), bl(false));
  //std::cout << format<std::string>(str("coucou"), idec(10302890), bl(false), str("\n"));

  // std::string s("coucou");
  // std::cout << format<std::string>("{}-{}", str(s), cstr("\n"));
  // std::cout << format<std::string>("{}-{}--{}---{}---", idec(1302), bl(false), cstr("true"), cstr("\n"));
}
