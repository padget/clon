#ifndef __clon_test_hpp__
#define __clon_test_hpp__

#include "format.hpp"
#include <iostream>

#define test_equals(actual, expected)                                  \
  std::cout << clon::fmt::format("===== at l.{} test {} == {} : {}\n", \
                                 __LINE__, #actual, #expected,         \
                                 ((actual) == (expected)) ? "OK" : "KO");

#define test_not_equals(actual, expected)                              \
  std::cout << clon::fmt::format("===== at l.{} test {} != {} : {}\n", \
                                 __LINE__, #actual, #expected,         \
                                 ((actual) != (expected)) ? "OK" : "KO");

#define run_test(testname)                                        \
  std::cout << clon::fmt::format("--------------------------\n"); \
  std::cout << clon::fmt::format("=  test file {}\n", __FILE__);  \
  std::cout << clon::fmt::format("=== run {}\n", #testname);      \
  testname();

#endif
