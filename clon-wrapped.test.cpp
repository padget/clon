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
    (level 13))
  (log 
    (level 14))))";

void first_try()
{
  using namespace clon::fmt;
  using namespace clon;

  std::string_view tosplit = "toto.titi.tata";

  auto spl = split(tosplit, '.');
  auto b = spl.begin();
  std::cout << *b << std::endl;
  ++b;
  std::cout << *b << std::endl;
  ++b;
  std::cout << *b << std::endl;
  ++b;
  std::cout << *b << std::endl;
}

#include <typeinfo>

int main(int argc, char **argv)
{

  using namespace clon::fmt;
  using namespace clon;
  std::string_view tosplit = "toto.titi:1.tata:4.";


  for (const auto &s : split_paths(tosplit))
    std::cout << format("{} : [{}-{}]\n", s.name, s.min, s.max);

  return EXIT_SUCCESS;
}
