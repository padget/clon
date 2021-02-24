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

struct local_string_view
{
  const char *data;
  std::size_t s;
};

void first_try()
{
  using namespace clon::fmt;
  using namespace clon;

  std::cout << format("{}\n", make_rview(parse(cl2)));
  // std::cout << clon::fmt::format("{}", r["log.level"].name());
  std::string_view pth = "log:1";
  std::cout << format("{}\n", parse_path(pth).min);
  std::cout << format("{}\n", getone(pth, make_rview(parse(cl2))));
}

#include <typeinfo>

int main(int argc, char **argv)
{
  run_test(first_try);
  return EXIT_SUCCESS;
}
