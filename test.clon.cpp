#include <iostream>
#include "clon.hpp"
#include "test.hpp"

constexpr std::string_view cl1 = R"(
  (log 
    (level "info")
    (message "a test object")))";

constexpr std::string_view cl2 = R"(
  (root 
    (log 
      (level "info")
      (level "fatal")
      (message "a test object"))
    (log 
      (level "info")
      (level "fatal"))))";

void cl1_should_be_parseable()
{
  
}

void root_name_should_be_equal_to_log()
{
  const clon::clon& c = clon::parse(cl1);

  test_equals(c.name, "log");
}

void root_clon_should_be_an_object()
{
  const clon::clon& c = clon::parse(cl1);
  
  test_equals(clon::is_object(c), true);
  test_equals(clon::is_string(c), false);
  test_equals(clon::is_none(c), false);
  test_equals(clon::is_bool(c), false);
  test_equals(clon::is_number(c), false);
}



void get_many_toto()
{
  const clon::clon& c = clon::parse(cl2);

  for (auto&& item : clon::get_all("log:*.level:*", c))
    std::cout << item.get().name << std::endl;
}

void get_toto()
{
  const clon::clon& c = clon::parse(cl2);
  for (auto&& item : clon::get_all("log:1", c))
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
          (level "fatal")))    )";

  auto res = clon::check("log.level", "s:2-3", clon::parse(cs));
  std::cout << std::boolalpha << res << '\n';
  std::cout << clon::check("log.level", "s:1-1", clon::parse(cs)) << '\n';
}

#include <typeinfo>

int main(int argc, char** argv)
{
  run_test(root_name_should_be_equal_to_log);
  run_test(root_clon_should_be_an_object);
  run_test(cl1_should_be_parseable);
  run_test(get_many_toto);
  run_test(check_test);
}
