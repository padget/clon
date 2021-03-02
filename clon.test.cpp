#include <iostream>
#include "clon.hpp"
#include "test.hpp"

constexpr std::string_view str = R"(
(bdd 
  (person 
    (name "Morreti")
    (firstname "Paul")
    (firstname "Jonhson")
    (firstname "Henry")
    (age 35)
    (male true)
    (female false)
    (adress
      (street "Shepard Harry Street")
      (postal 82910)
      (city "Manchester")))
   (person 
    (name "Londubass")
    (firstname "Gordon")
    (firstname "Morizion")
    (age 86)
    (male true)
    (female false)
    (adress
      (street "Blueprint Street")
      (postal 56468)
      (city "London")))))";

constexpr std::string_view stre = R"()";

void should_parse()
{
  clon::api a(str);

  test_equals(a.name(), "bdd");
  test_equals(a.total_length(), 24);
}

void should_catch_exception()
{
  test_catch(clon::api(stre), std::runtime_error);
}

int main(int argc, char **argv)
{

  using namespace clon::fmt;
  using namespace clon;

  run_test(should_parse);
  run_test(should_catch_exception);

  return EXIT_SUCCESS;
}
