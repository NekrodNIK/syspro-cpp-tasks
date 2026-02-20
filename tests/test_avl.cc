#include "../src/avl.hpp"
#include <gtest/gtest.h>

using lib::AvlOrderedSet;

TEST(AvlOrderedSetSuite, EmptySetTest) {
  AvlOrderedSet<int> set;
  EXPECT_EQ(set.begin(), set.end());
}

TEST(AvlOrderedSetSuite, InsertTest) {
  AvlOrderedSet<int> set;
  set.insert(42);
  EXPECT_NE(set.find(42), set.end());
}

TEST(AvlOrderedSetSuite, NotFoundTest) {
  AvlOrderedSet<int> set;
  set.insert(42);
  EXPECT_EQ(set.find(43), set.end());
}

TEST(AvlOrderedSetSuite, InsertMultipleTest) {
  AvlOrderedSet<int> set;

  set.insert(42);
  set.insert(43);
  set.insert(44);

  EXPECT_NE(set.find(42), set.end());
  EXPECT_NE(set.find(43), set.end());
  EXPECT_NE(set.find(44), set.end());
}

TEST(AvlOrderedSetSuite, InsertDuplicateTest) {
  AvlOrderedSet<int> set;
  set.insert(42);
  set.insert(42);

  int cnt = 0;
  for (auto it = set.begin(); it != set.end(); ++it)
    cnt++;
  EXPECT_EQ(cnt, 1);
}

TEST(AvlOrderedSetSuite, IteratorIncTest) {
  AvlOrderedSet<int> set;
  set.insert(42);
  set.insert(41);
  set.insert(43);

  auto it = set.begin();
  EXPECT_EQ(*(it++), 41);
  EXPECT_EQ(*(it++), 42);
  EXPECT_EQ(*(it++), 43);
  EXPECT_EQ(it, set.end());
}

TEST(AvlOrderedSetSuite, IteratorDecTest) {
  AvlOrderedSet<int> set;
  set.insert(43);
  set.insert(41);
  set.insert(42);

  auto it = set.end();
  EXPECT_EQ(*(--it), 43);
  EXPECT_EQ(*(--it), 42);
  EXPECT_EQ(*(--it), 41);
  EXPECT_EQ(it, set.begin());
}

TEST(AvlOrderedSetSuite, RemoveTest) {
  AvlOrderedSet<int> set;
  set.insert(42);
  set.insert(43);

  set.remove(42);
  EXPECT_EQ(set.find(42), set.end());
  EXPECT_NE(set.find(43), set.end());
}

TEST(AvlOrderedSetSuite, RemoveLastTest) {
  AvlOrderedSet<int> set;
  set.insert(42);
  set.remove(42);
  EXPECT_EQ(set.begin(), set.end());
}

TEST(AvlOrderedSetSuite, UpperBoundTest) {
  AvlOrderedSet<int> set;
  set.insert(10);
  set.insert(20);

  EXPECT_EQ(*set.upper_bound(15), 20);
  EXPECT_EQ(set.upper_bound(30), set.end());
}

TEST(AvlOrderedSetSuite, StringsTest) {
  AvlOrderedSet<std::string> set;
  set.insert("DON'T");
  set.insert("PANIC");

  EXPECT_EQ(*set.begin(), "DON'T");
  EXPECT_NE(set.find("PANIC"), set.end());
}
