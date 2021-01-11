#include <iostream>
#include "clon.hpp"


#define test_equals(actual, expected)              \
  fmt::print("===== at l.{} test {} == {} : {}\n", \
    __LINE__, #actual, #expected,                  \
    ((actual) == (expected)) ? "OK" : "KO");       \

#define test_not_equals(actual, expected)          \
  fmt::print("===== at l.{} test {} != {} : {}\n", \
    __LINE__, #actual, #expected,                  \
    ((actual) != (expected)) ? "OK" : "KO");       \

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

  test_equals(c.name, "log");
  test_equals(clon::is_object(c), true);
  test_equals(clon::is_string(c), false);
  test_equals(clon::is_none(c), false);
  test_equals(clon::is_bool(c), false);
  test_equals(clon::is_number(c), false);
}

void get_many_toto()
{
  std::string cs =
    R"((root 
        (log 
          (level "info")
          (level "fatal")
          (message "a test object"))
        (log 
          (level "info")
          (level "fatal"))))";

  const clon::clon& c = clon::parse(cs);
  
  for (auto&& item : clon::get_all("log:*.level:*", c))
    std::cout << item.get().name << std::endl;
}

void check_test() 
{
  std::string_view cs =
    R"((root 
        (log 
          (level "info")
          (level "fatal")
          (message "a test object"))
        (log 
          (level "info")
          (level "fatal"))))";
 
  auto res = clon::check("log.level", "s:2-3", clon::parse(cs));
  std::cout << std::boolalpha << res << '\n';
  std::cout << clon::check("log.level", "s:1-1", clon::parse(cs)) << '\n';
}

int main(int argc, char** argv)
{
  run_test(root_name_equals);
  run_test(get_many_toto);
  run_test(check_test);
}
