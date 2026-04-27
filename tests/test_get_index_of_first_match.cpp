#include "src/get_index_of_first_match.hpp"
#include <cstring>
#include <gtest/gtest.h>

TEST(GetIndexOfFirstMatchSuite, SimpleTest) {
  auto ind = getIndexOfFirstMatch([](auto x) { return x == 1; }, 0, 8, 3, 1, 2,
                                  1, 1, 3, 1);
  GTEST_ASSERT_EQ(ind, 3);
  ind = getIndexOfFirstMatch([](auto x) { return x == 42; }, 0, 8, 3, 1, 2, 1,
                             1, 3, 1);
  GTEST_ASSERT_EQ(ind, -1);
  ind = getIndexOfFirstMatch([](auto x) { return x == 2; }, 0, 8, 3, 1, 2, 1, 1,
                             3, 1);
  GTEST_ASSERT_EQ(ind, 4);
}

template <typename T>
bool compare(T&) {
  return false;
}

bool compare(const char*& x) { return std::strcmp(x, "DON'T PANIC") == 0; }

TEST(GetIndexOfFirstMatchSuite, MixedTypesTest) {
  auto ind = getIndexOfFirstMatch(
      [](auto x) {
        if constexpr (std::is_same<decltype(x), const char*>()) {
          return std::strcmp(x, "DON'T PANIC") == 0;
        } else {
          return false;
        }
      },
      0, "42", 3, "DON'T PANIC", 2, "str", 1, 3, 2, 1);
  GTEST_ASSERT_EQ(ind, 3);
  ind = getIndexOfFirstMatch(
      [](auto x) {
        if constexpr (std::is_same<decltype(x), int>()) {
          return x == 11;
        } else {
          return false;
        }
      },
      0, "42", 3, "DON'T PANIC", 2, "str", 1, 3, 2, 1, 11);
  GTEST_ASSERT_EQ(ind, 10);
}
