#include "src/container.hpp"
#include <any>
#include <gtest/gtest.h>
#include <string>

TEST(ContainerSuite, SimpleTest) {
  container<int, char, std::string> c(1, '2', "hello");
  auto x = c.getElement<int>(0);
  auto y = c.getElement<char>(1);
  auto z = c.getElement<std::string>(2);
  ASSERT_EQ(x, 1);
  ASSERT_EQ(y, '2');
  ASSERT_EQ(z, std::string("hello"));

  auto w = c.getElement(0);
  ASSERT_EQ(std::any_cast<int>(w), 1);

  auto q = c.getElement<2>();
  ASSERT_EQ(q, std::string("hello"));
}
