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

void first_try()
{
  clon::root<char> r(cl2);
  std::size_t index = 0;

  for (auto &&node : r.nodes)
  {
    std::cout << clon::fmt::format(
        "index : {}, name : {}, valv: {}, child: {}, nexts {}\n",
        index++, node.name(), node.valv(), node.child(), node.next());

    std::cout << "type :" << ((unsigned)node.type()) << "\n";

    switch (node.type())
    {
    case clon::clon_type::boolean:
      std::cout << node.as_<clon::boolean>() << '\n';
      break;
    case clon::clon_type::number:
      std::cout << node.as_<clon::number>() << '\n';
      break;
    case clon::clon_type::string:
      std::cout << node.as_<clon::string<char>>() << '\n';
      break;
    default:
      break;
    }
  }
}

#include <typeinfo>

int main(int argc, char **argv)
{
  run_test(first_try);
}
