#include "../src/avl.hpp"
#include <gtest/gtest.h>

using cpp_utils::AvlTreeSet;

TEST(AvlTreeSetSuite, EmptySetTest) {
  AvlTreeSet<int> set;
  EXPECT_EQ(set.begin(), set.end());
}

TEST(AvlTreeSetSuite, InsertTest) {
  AvlTreeSet<int> set;
  set.insert(42);
  EXPECT_NE(set.find(42), set.end());
}

TEST(AvlTreeSetSuite, NotFoundTest) {
  AvlTreeSet<int> set;
  set.insert(42);
  EXPECT_EQ(set.find(43), set.end());
}

TEST(AvlTreeSetSuite, InsertMultipleTest) {
  AvlTreeSet<int> set;

  set.insert(42);
  set.insert(43);
  set.insert(44);

  EXPECT_NE(set.find(42), set.end());
  EXPECT_NE(set.find(43), set.end());
  EXPECT_NE(set.find(44), set.end());
}

TEST(AvlTreeSetSuite, InsertDuplicateTest) {
  AvlTreeSet<int> set;
  set.insert(42);
  set.insert(42);

  int cnt = 0;
  for (auto it = set.begin(); it != set.end(); ++it)
    cnt++;
  EXPECT_EQ(cnt, 1);
}

TEST(AvlTreeSetSuite, IteratorIncTest) {
  AvlTreeSet<int> set;
  set.insert(42);
  set.insert(41);
  set.insert(43);

  auto it = set.begin();
  EXPECT_EQ(*(it++), 41);
  EXPECT_EQ(*(it++), 42);
  EXPECT_EQ(*(it++), 43);
  EXPECT_EQ(it, set.end());
}

TEST(AvlTreeSetSuite, IteratorDecTest) {
  AvlTreeSet<int> set;
  set.insert(43);
  set.insert(41);
  set.insert(42);

  auto it = set.end();
  EXPECT_EQ(*(--it), 43);
  EXPECT_EQ(*(--it), 42);
  EXPECT_EQ(*(--it), 41);
  EXPECT_EQ(it, set.begin());
}

TEST(AvlTreeSetSuite, RemoveTest) {
  AvlTreeSet<int> set;
  set.insert(42);
  set.insert(43);

  set.remove(set.find(42));
  EXPECT_EQ(set.find(42), set.end());
  EXPECT_NE(set.find(43), set.end());
}

TEST(AvlTreeSetSuite, RemoveLastTest) {
  AvlTreeSet<int> set;
  set.insert(42);
  set.remove(set.find(42));
  EXPECT_EQ(set.begin(), set.end());
}

TEST(AvlTreeSetSuite, UpperBoundTest) {
  AvlTreeSet<int> set;
  set.insert(10);
  set.insert(20);

  EXPECT_EQ(*set.upperBound(15), 20);
  EXPECT_EQ(set.upperBound(30), set.end());
}

TEST(AvlTreeSetSuite, StringsTest) {
  AvlTreeSet<std::string> set;
  set.insert("DON'T");
  set.insert("PANIC");

  EXPECT_EQ(*set.begin(), "DON'T");
  EXPECT_NE(set.find("PANIC"), set.end());
}

