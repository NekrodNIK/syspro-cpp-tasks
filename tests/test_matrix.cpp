#include "src/matrix.hpp"
#include <gtest/gtest.h>

TEST(MatrixSuite, Simple) {
  Matrix<int> matrix(42, 5, 10);
  EXPECT_EQ(matrix.rows(), 5);
  EXPECT_EQ(matrix.cols(), 10);
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < 10; j++)
      EXPECT_EQ(matrix[i][j], 42);
  matrix[1][1] = 43;
  EXPECT_EQ(matrix[1][1], 43);
  EXPECT_EQ(matrix(10 + 1), 43);
}

TEST(MatrixSuite, DiagonalCstr) {
  std::vector<std::string> diag = {"DON'T", " ", "PANIC!"};
  Matrix<std::string> matrix(diag);
  EXPECT_EQ(matrix.rows(), 3);
  EXPECT_EQ(matrix.cols(), 3);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (i == j) {
        EXPECT_EQ(matrix[i][j], diag[i]);
      } else {
        EXPECT_EQ(matrix[i][j], "");
      }
    }
  }
}

TEST(MatrixSuite, Map) {
  Matrix<int> m(42, 2, 2);
  auto result = m.map([](const int& x) { return x * 10; });

  EXPECT_EQ(result.rows(), 2);
  EXPECT_EQ(result.cols(), 2);
  EXPECT_DOUBLE_EQ(result[0][0], 420);
  EXPECT_DOUBLE_EQ(result[0][1], 420);
  EXPECT_DOUBLE_EQ(result[1][0], 420);
  EXPECT_DOUBLE_EQ(result[1][1], 420);
}

TEST(MatrixSuite, MapDifferentType) {
  Matrix<int> m(42, 1, 3);
  Matrix<std::string> result = m.map([](auto x) { return std::format("DON'T PANIC! {}", x); });
  EXPECT_EQ(result[0][0], "DON'T PANIC! 42");
  EXPECT_EQ(result[0][1], "DON'T PANIC! 42");
  EXPECT_EQ(result[0][2], "DON'T PANIC! 42");
}

TEST(MatrixSuite, PlusOperator) {
  Matrix<int> a(1, 2, 3);
  Matrix<int> b(42, 2, 3);
  auto c = a + b;
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      EXPECT_EQ(c[i][j], a[i][j] + b[i][j]);
    }
  }
  auto d = a + 1000;
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      EXPECT_EQ(d[i][j], a[i][j] + 1000);
    }
  }
}

TEST(MatrixSuite, MultipliesOperator) {
  Matrix<int> a(1, 2, 3);
  Matrix<int> b(2, 3, 2);
  auto c = a * b;
  EXPECT_EQ(c.rows(), 2);
  EXPECT_EQ(c.cols(), 2);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      EXPECT_EQ(c[i][j], 6);
    }
  }
  auto d = a * 4;
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      EXPECT_EQ(d[i][j], a[i][j] * 4);
    }
  }
}

TEST(MatrixSuite, ExceptionOnInvalidMultiplication) {
  Matrix<int> a(1, 2, 3);
  Matrix<int> b(1, 4, 2);
  EXPECT_THROW({ auto c = a * b; }, std::invalid_argument);
}
