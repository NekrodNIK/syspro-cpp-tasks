#include "src/instance_limiter.hpp"
#include <array>
#include <gtest/gtest.h>

constexpr auto max = 20;
class Limited : public InstanceLimiterMixin<Limited, max> {};

TEST(InstanceLimiterSuite, TestLimitedDefaultConstructor) {
  std::array<Limited, 19> arr;
  Limited _;
  ASSERT_THROW(Limited(), limit_error);
}

TEST(InstanceLimiterSuite, TestLimitedCopyConstructor) {
  std::array<Limited, 20> arr1;
  ASSERT_THROW(auto arr2 = arr1, limit_error);
}

TEST(InstanceLimiterSuite, TestLimitedDestructor) {
  { std::array<Limited, 20> arr1; }
  Limited();
}

class Singleton : public SingletonLimiterMixin<Singleton> {};

TEST(SingletonLimiterSuite, TestSingletonDefaultConstructor) {
  Singleton x;
  ASSERT_THROW(Singleton y, limit_error);
}

TEST(SingletonLimiterSuite, TestSingletonCopyConstructor) {
  Singleton x;
  ASSERT_THROW(Singleton y = x, limit_error);
}

TEST(SingletonLimiterSuite, TestSingletonDestructor) {
  { Singleton x; }
  Singleton y;
}
