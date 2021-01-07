#include "clon.hpp"
#include <iostream>

#define test_equals(actual, expected)              \
  fmt::print("===== at l.{} test {} == {} : {}\n", \
    __LINE__, #actual, #expected,                  \
    ((actual) == (expected)) ? "OK" : "KO");       \


#define run_test(testname)                   \
  fmt::print("--------------------------\n");\
  fmt::print("=  test file {}\n", __FILE__); \
  fmt::print("=== run {}\n", #testname);     \
  testname();                                \


void root_name_equals()
{
  std::string cs =
    R"((log 
        (level "info")
        (message "a test object")))";

  const clon::clon& c = clon::parse(cs);

  test_equals(c.name, "log2");
  test_equals(clon::is_object(c), true);
  test_equals(clon::is_string(c), false);
  test_equals(clon::is_none(c), false);
  test_equals(clon::is_bool(c), false);
  test_equals(clon::is_number(c), false);
}

int main(int argc, char** argv)
{
  run_test(root_name_equals);
}
