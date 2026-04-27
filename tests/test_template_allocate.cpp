#include "src/template-allocate.hpp"
#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>
#include <utility>

TEST(TemplateAllocateSuite, SimpleTest) {
  char arr[100];
  [&]<size_t... I>(std::index_sequence<I...>) {
    allocate<100>(arr, static_cast<char>(I)...);
  }(std::make_index_sequence<100>());
  for (int i = 0; i < 100; i++)
    ASSERT_EQ(arr[i], i);
}

TEST(TemplateAllocateSuite, DifferentTypesTest) {
  char arr[5];
  char b = 11;
  allocate<5>(arr, '1', static_cast<uint16_t>(10), static_cast<char>(5), b);
  ASSERT_EQ(arr[0], '1');
  uint16_t v = 10;
  ASSERT_EQ(std::memcmp(arr + 1, &v, 2), 0);
  ASSERT_EQ(arr[3], 5);
  ASSERT_EQ(arr[4], 11);
}
