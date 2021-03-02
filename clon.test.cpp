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
    (address
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
    (address
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

void should_equals_to_82910()
{
  clon::api a(str);
  test_equals(a["person.address.postal"].type(), clon::clon_type::number);
  test_equals(a["person.address.postal"].as_<clon::number>(), 82910);
  test_equals(a.number("person.address.postal"), 82910);
}

void should_equals_to_true()
{
  clon::api a(str);
  test_equals(a["person.male"].type(), clon::clon_type::boolean);
  test_equals(a["person.male"].as_<clon::boolean>(), true);
  test_equals(a["person:1.male"].type(), clon::clon_type::boolean);
  test_equals(a.boolean("person:1.male"), true);
}

void should_equals_to_false()
{
  clon::api a(str);
  test_equals(a["person.female"].type(), clon::clon_type::boolean);
  test_equals(a["person.female"].as_<clon::boolean>(), false);
  test_equals(a["person:1.female"].type(), clon::clon_type::boolean);
  test_equals(a["person:1.female"].as_<clon::boolean>(), false);
  test_equals(a.boolean("person:1.female"), false);
}

void should_equals_to_Paul()
{
  clon::api a(str);
  test_equals(a["person.firstname"].type(), clon::clon_type::string);
  test_equals(a["person.firstname"].as_<clon::string<char>>(), "Paul");
  test_equals(a.string("person.firstname"), "Paul");
  
}

int main(int argc, char **argv)
{

  using namespace clon::fmt;
  using namespace clon;

  run_test(should_parse);
  run_test(should_catch_exception);
  run_test(should_equals_to_82910);
  run_test(should_equals_to_true);
  run_test(should_equals_to_false);
  run_test(should_equals_to_Paul);

  return EXIT_SUCCESS;
}
