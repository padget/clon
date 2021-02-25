#include <iostream>
#include "clon.hpp"
#include "test.hpp"

constexpr std::string_view cl2 = R"(
(root 
  (log 
    (level 
      (message "bonne nuit les petits"))
    (level 10)
    (level 11)) 
  (log 
    (level 13))
  (log 
    (level 14))))";

void first_try()
{
  using namespace clon::fmt;
  using namespace clon;

  api a(cl2);
  std::cout << fmt::format("{}", a);
}

#include <typeinfo>

int main(int argc, char **argv)
{

  using namespace clon::fmt;
  using namespace clon;

  first_try();

  return EXIT_SUCCESS;
}
