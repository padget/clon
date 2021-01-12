#ifndef __clon_test_hpp__
#define __clon_test_hpp__

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

#endif
