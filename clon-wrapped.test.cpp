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
  const char *data;
  std::size_t s;
};

void first_try()
{
  std::cout << clon::fmt::format("{}\n", clon::make_rview(clon::parse(cl2)));
  // std::cout << clon::fmt::format("{}", r["log.level"].name());
}

#include <typeinfo>

int main(int argc, char **argv)
{
  run_test(first_try);
  return EXIT_SUCCESS;
}
