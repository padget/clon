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

struct local_string_view
{
  const char * data;
  std::size_t s;
};

void first_try()
{
  clon::root<char> r(cl2);

  std::cout << clon::fmt::format("{}\n", r.view());
  std::cout << clon::fmt::format("{}\n", r.view(7));
  std::cout << clon::fmt::format("{}\n", r.view(6));

  std::cout << clon::fmt::format("size_t max : {}\n", std::numeric_limits<std::size_t>::max());
  std::cout <<  "long double max : " << std::numeric_limits<long double>::epsilon() << '\n';
  std::cout << clon::fmt::format("size of node : {}\n", sizeof(clon::node<char>));
  std::cout << clon::fmt::format("size of std::bsv {}\n", sizeof(std::basic_string_view<char>));
  std::cout << clon::fmt::format("size of lsv {}\n", sizeof(local_string_view));
  std::cout << clon::fmt::format("size of size_t {}\n", sizeof(std::size_t));
  std::cout << clon::fmt::format("size of char* {}\n", sizeof(char*));
  std::cout << clon::fmt::format("size of double {}\n", sizeof(long double));
  std::cout << clon::fmt::format("size of value {}\n", sizeof(clon::value<char>));
}


#include <typeinfo>

int main(int argc, char **argv)
{
  run_test(first_try);
  return EXIT_SUCCESS;
}
