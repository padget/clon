#include "model.hpp"

clon::model::node& clon::basic::undefined()
{
  static clon::model::node c;
  return c;
}

clon::model::clon_type clon::basic::type(const clon::model::node& c)
{
  switch (c.val.index())
  {
  case 0:
    return clon::model::clon_type::none;
  case 1:
    return clon::model::clon_type::boolean;
  case 2:
    return clon::model::clon_type::number;
  case 3:
    return clon::model::clon_type::string;
  case 4:
    return clon::model::clon_type::object;
  default:
    return clon::model::clon_type::none;
  }
}
