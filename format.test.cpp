#include <iostream>

#include "format.hpp"
#include "test.hpp"

using namespace clon::fmt;

void double_to_string()
{
  double d = 12.6546;
  // retrouver la partie entière d'un double
  std::cout << int(d) << std::endl;

  double dec = d - int(d);

  while (dec > 2e-10)
  {
    dec = dec * 10;
    std::cout << dec << " -> ";
    dec = dec - int(dec);
    std::cout << dec << std::endl;
  }
}
void should_format()
{
  test_equals(format("{};{};{}", 1, 2, 3), "1;2;3");
  test_equals(format("{}", 1, 2, 3), "123");
  test_equals(format("{}{}{}-{}", 1, 2, 3), "123-");
  test_equals(format("{}{}{}{}", 1, 2, 3), "123");
  test_equals(format("{}_{}", std::string("co"), std::string("co")), "co_co");
  test_equals(format("{}", std::vector<char>({'c', 'o', 'u'})), "cou");
}

int main(int argc, char **argv)
{
  double_to_string();

  run_test(should_format);
}