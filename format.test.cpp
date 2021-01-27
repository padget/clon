#include <iostream>
#include "format.hpp" 


#include <string_view>
#include <vector>

int main()
{
  namespace fmt = clon::fmt;
  using namespace fmt; 

  //std::cout << format<std::string>("{}{}{} \n", str("coucou"), idec(10302890), bl(false));
  //std::cout << format<std::string>(str("coucou"), idec(10302890), bl(false), str("\n"));
  std::cout << format<std::string>(str("coucou"), idec(13021989), bl(false), str("\n")); 
}
