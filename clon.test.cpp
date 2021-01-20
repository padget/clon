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

void undefined_should_be_none()
{
  test_equals(clon::type(clon::undefined()), clon::clon_type::none);
  test_equals(clon::undefined().name, "");
  test_equals(clon::undefined().val.index(), 0);
}

void type_should_be_string()
{
  clon::node c = { "name", "coucou" };
  test_equals(clon::type(c), clon::clon_type::string);
}

void type_should_be_number()
{
  clon::node c = { "name", 12. };
  test_equals(clon::type(c), clon::clon_type::number);
}

void type_should_be_object()
{
  clon::node item = { "sub", "string" };
  clon::node c = { "name", std::vector<clon::node>{item} };
  test_equals(clon::type(c), clon::clon_type::object);
}

void type_should_be_boolean()
{
  clon::node c = { "name", true };
  test_equals(clon::type(c), clon::clon_type::boolean);
}

void is_should_return_true_if_string()
{
  clon::node c = { "name", "coucou" };
  test_equals(clon::is_<clon::string>(c), true);
}

void is_should_return_true_if_number()
{
  clon::node c = { "name", 12. };
  test_equals(clon::is_<clon::number>(c), true);
}

void is_should_return_true_if_object()
{
  clon::node item = { "sub", "string" };
  clon::node c = { "name", std::vector<clon::node>{item} };
  test_equals(clon::is_<clon::object>(c), true);
}

void is_should_return_true_if_boolean()
{
  clon::node c = { "name", true };
  test_equals(clon::is_<clon::boolean>(c), true);
}

void is_should_return_true_if_none()
{
  test_equals(clon::is_<clon::none>(clon::undefined()), true);
}

void as_should_return_correct_string()
{
  clon::node c = { "name", "coucou" };
  test_equals(clon::as_<clon::string>(c), "coucou");
}

void as_should_return_correct_number()
{
  clon::node c = { "name", 12. };
  test_equals(clon::as_<clon::number>(c), 12.);
}

void as_should_return_correct_boolean()
{
  clon::node c = { "name", true };
  test_equals(clon::as_<clon::boolean>(c), true);
}

void as_should_return_correct_object()
{
  clon::node item = { "sub", "string" };
  clon::node c = { "name", std::vector<clon::node>{item} };
  test_equals(clon::as_<clon::object>(c).size(), 1);
  test_equals(clon::as_<clon::object>(c).at(0).name, "sub");
  test_equals(clon::is_<clon::string>(clon::as_<clon::object>(c).at(0)), true);
}

void get_should_return_string_node()
{
  using namespace clon;
  node age = { "age", 12. };
  node name = { "name", "doe" };
  node name2 = { "name", "wilson" };
  node firstname = { "firstname", "marsel" };
  object lst = { firstname, name, name2, age };
  node person = { "person", lst };

  test_equals(is_<string>(get("name", person)), true);
  test_equals(as_<string>(get("name", person)), "doe");
  test_equals(is_<string>(get("name:0", person)), true);
  test_equals(as_<string>(get("name:0", person)), "doe");
  test_equals(is_<string>(get("name:1", person)), true);
  test_equals(as_<string>(get("name:1", person)), "wilson");
}

void get_should_return_number_node()
{
  using namespace clon;
  node age = { "age", 12. };
  node name = { "name", "doe" };
  node name2 = { "name", "wilson" };
  node firstname = { "firstname", "marsel" };
  object lst = { firstname, name, name2, age };
  node person = { "person", lst };

  test_equals(is_<number>(get("age", person)), true);
  test_equals(as_<number>(get("age", person)), 12.);
  test_equals(is_<number>(get("age:0", person)), true);
  test_equals(as_<number>(get("age:0", person)), 12.);
}

void get_should_return_boolean_node()
{
  using namespace clon;
  node age = { "age", 12. };
  node name = { "name", "doe" };
  node alive = { "alive", true };
  node firstname = { "firstname", "marsel" };
  object lst = { firstname, name, alive, age };
  node person = { "person", lst };

  test_equals(is_<boolean>(get("alive", person)), true);
  test_equals(as_<boolean>(get("alive", person)), true);
  test_equals(is_<boolean>(get("alive:0", person)), true);
  test_equals(as_<boolean>(get("alive:0", person)), true);
}

void get_complex_path_should_return_number_node()
{
  using namespace clon;
  node street = { "street", "wilson avenue" };
  node nb = { "number", 4.};
  object addrlst = { street, nb };
  node adress = { "address" , addrlst };
  node age = { "age", 12. };
  node name = { "name", "doe" };
  node name2 = { "alive", true };
  node firstname = { "firstname", "marsel" };
  object lst = { firstname, name, name2, age , adress };
  node person = { "person", lst };

  test_equals(is_<number>(get("address.number", person)), true);
  test_equals(as_<number>(get("address.number", person)), 4);
  test_equals(is_<number>(get("address.number:0", person)), true);
  test_equals(as_<number>(get("address.number:0", person)), 4);
}

void get_complex_path_should_return_string_node()
{
  using namespace clon;
  node street = { "street", "wilson avenue" };
  node nb = { "number", 4. };
  object addrlst = { street, nb };
  node adress = { "address" , addrlst };
  node age = { "age", 12. };
  node name = { "name", "doe" };
  node name2 = { "alive", true };
  node firstname = { "firstname", "marsel" };
  object lst = { firstname, name, name2, age , adress };
  node person = { "person", lst };

  test_equals(is_<string>(get("address.street", person)), true);
  test_equals(as_<string>(get("address.street", person)), "wilson avenue");
  test_equals(is_<string>(get("address.street:0", person)), true);
  test_equals(as_<string>(get("address.street:0", person)), "wilson avenue");
}

void get_all_should_return_string_nodes()
{
  using namespace clon;
  node age = { "age", 12. };
  node name = { "name", "doe" };
  node name2 = { "name", "wilson" };
  node firstname = { "firstname", "marsel" };
  object lst = { firstname, name, name2, age };
  node person = { "person", lst };
  test_equals(get_all("name:*", person).size(), 2);
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
  run_test(undefined_should_be_none);
  run_test(type_should_be_boolean);
  run_test(type_should_be_number);
  run_test(type_should_be_object);
  run_test(type_should_be_string);
  run_test(is_should_return_true_if_string);
  run_test(is_should_return_true_if_number);
  run_test(is_should_return_true_if_object);
  run_test(is_should_return_true_if_boolean);
  run_test(is_should_return_true_if_none);
  run_test(as_should_return_correct_object);
  run_test(as_should_return_correct_string);
  run_test(as_should_return_correct_boolean);
  run_test(as_should_return_correct_number);
  run_test(get_should_return_string_node);
  run_test(get_should_return_number_node);
  run_test(get_should_return_boolean_node);
  // run_test(get_should_return_object_node);
  run_test(get_complex_path_should_return_number_node);
  run_test(get_complex_path_should_return_string_node);
  run_test(get_all_should_return_string_nodes);

}
