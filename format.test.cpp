
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

  pattern<const char> p = init_pattern("cou{}hqlskdj{}ddds{}qsldj{}qsldj{}");
 
  for (int i = 0; i < 6; i++)
  {
    std::cout << '"';
    for (unsigned j = 0; j < p.spans().data[i].size(); j++)
      std::cout << p.spans().data[i][j];
    std::cout << '"' << std::endl;
  }

  std::cout << p.size() << std::endl;
  span<const char> s = init_cspan("coucou");
  span<const char> s2 = init_cspan("ou");
  std::cout  << find(s, s2);



  //std::cout << format<std::string>("{}{}{} \n", str("coucou"), idec(10302890), bl(false));
  //std::cout << format<std::string>(str("coucou"), idec(10302890), bl(false), str("\n"));

  // std::string s("coucou");
  // std::cout << format<std::string>("{}-{}", str(s), cstr("\n"));
  // std::cout << format<std::string>("{}-{}--{}---{}---", idec(1302), bl(false), cstr("true"), cstr("\n"));
}
