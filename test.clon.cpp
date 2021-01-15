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
  const clon::root& c = clon::parse(cl1);

  test_equals(c.root.name, "log");
}

void root_should_be_an_object()
{
  const clon::root& c = clon::parse(cl1);

  test_equals(clon::is_<clon::object>(c.root), true);
  test_equals(clon::is_<clon::string>(c.root), false);
  test_equals(clon::is_<clon::none>(c.root), false);
  test_equals(clon::is_<clon::boolean>(c.root), false);
  test_equals(clon::is_<clon::number>(c.root), false);
}

void to_string_must_fonctionne()
{
  const clon::root& c = clon::parse(cl1);
  std::cout << clon::to_string(c.root) << std::endl;
}

void to_original_string_must_fonctionne()
{
  const clon::root& c = clon::parse(cl1);
  std::cout << clon::to_original_string(c) << std::endl;
}

void to_clon_must_fonctionne()
{
  struct person
  {
    std::string name;
    std::string firstname;
    std::size_t age;
    std::vector<std::string> secondnames;
  };

  // auto to_clon = [](const person& p)
  // {
  //   clon::temporary_buffer tbuff;

  //   clon::to_clon(tbuff, "person",
  //     clon::pair("name", p.name),
  //     clon::pair("firstname", p.firstname),
  //     clon::pair("age", p.age),
  //     clon::sequence("secondname",
  //       p.secondnames.begin(), p.secondnames.end()));

  //   return clon::parse(fmt::to_string(tbuff));
  // };

  // person p = { "padget", "pro", 32, {"jhon", "fidgeral"} };

  // clon::root pc = to_clon(p);
  // std::cout << clon::to_original_string(pc) << '\n';
  // std::cout << clon::to_string(pc.root) << '\n';
  // std::cout << clon::to_string(clon::parse(clon::to_string(pc.root)).root) << '\n';
}

void get_many_toto()  
{
  const clon::root& c = clon::parse(cl2);

  for (auto&& item : clon::get_all("log:*.level:*", c.root))
    std::cout << item.get().name << std::endl;
}

void get_toto()
{
  const clon::root& c = clon::parse(cl2);
  for (auto&& item : clon::get_all("log:1", c.root))
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

  auto res = clon::check("log.level", "s:2-3", clon::parse(cs).root);
  std::cout << std::boolalpha << res << '\n';
  std::cout << clon::check("log.level", "s:1-1", clon::parse(cs).root) << '\n';
}

#include <typeinfo>

int main(int argc, char** argv)
{
  run_test(root_name_should_be_equal_to_log);
  run_test(root_should_be_an_object);
  run_test(cl1_should_be_parseable);
  run_test(to_string_must_fonctionne);
  run_test(to_original_string_must_fonctionne);
  run_test(to_clon_must_fonctionne);
  run_test(get_many_toto);
  run_test(check_test);
}
