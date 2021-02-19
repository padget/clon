#include <iostream>
#include "clon-wrapped.hpp"
#include "test.hpp"

constexpr std::string_view cl2 = R"(
(root 
  (log 
    (level 12)
    (level 12)
    (level 12)) 
  (log 
    (level 12))
  (log 
    (level 12))))";

void first_try()
{
  clon::root<char> r(cl2);

  std::cout << clon::fmt::format("{}\n", r);
  std::cout << clon::fmt::format("{}\n", clon::root_view<char, clon::parser<char>>(r, 7));
  std::cout << clon::fmt::format("{}\n", clon::root_view<char, clon::parser<char>>(r, 6));

  
}

#include <typeinfo>

int main(int argc, char **argv)
{
  run_test(first_try);
  return EXIT_SUCCESS;
}
