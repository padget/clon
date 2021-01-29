
#include "format.hpp"
#include <iostream>

#include <string_view>
#include <vector>

int main()
{
  namespace fmt = clon::fmt;
  using namespace fmt;
  pattern<const char, 3> p(chars_span<const char>("cou{}cou{}cou{}"));
  const lengths<3> lens{{12, 5, 15}};
  const coordinates<3> idx = p.get_idx(lens);

  for (int i = 0; i < 3; ++i)
    std::cout << idx.data[i] << '-';

  //std::cout << format<std::string>("{}{}{} \n", str("coucou"), idec(10302890), bl(false));
  //std::cout << format<std::string>(str("coucou"), idec(10302890), bl(false), str("\n"));

  std::string s("coucou");
  std::cout << format<std::string>(str(s), cstr("\n"));
  std::cout << format<std::string>(idec(1302), bl(false), cstr("true"), cstr("\n"));
}
