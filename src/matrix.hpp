#pragma once
#include <algorithm>
#include <cassert>
#include <concepts>
#include <numeric>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T>
class Matrix {
  int rows_;
  int cols_;
  std::vector<T> mem_;
  template <typename V>
  friend class Matrix;

public:
  Matrix(const Matrix&) = default;
  Matrix(Matrix&&) = default;
  Matrix& operator=(const Matrix&) = default;
  Matrix& operator=(Matrix&&) = default;

  Matrix(T initial, int rows, int cols)
      : rows_(rows), cols_(cols), mem_(rows * cols, initial) {}

  Matrix(int rows, int cols) : rows_(rows), cols_(cols), mem_(rows * cols) {}

  Matrix(const std::vector<T>& diagonal)
      : rows_(diagonal.size()), cols_(diagonal.size()), mem_(rows_ * cols_) {
    for (size_t i = 0; i < diagonal.size(); i++)
      (*this)[i][i] = diagonal[i];
  }

  int rows() const { return rows_; }
  int cols() const { return cols_; }

  T& operator()(int index) { return mem_[index]; }
  const T& operator()(int index) const { return mem_[index]; }
  std::span<T> operator[](int index) {
    return std::span(mem_).subspan(index * cols_, cols_);
  }
  std::span<const T> operator[](int index) const {
    return std::span(mem_).subspan(index * cols_, cols_);
  }

  explicit operator T() const {
    return std::accumulate(mem_.begin(), mem_.end(), T());
  }

  template <std::invocable<const T&> F,
            typename U = std::invoke_result_t<F, const T&>>
  Matrix<U> map(F&& f) const {
    Matrix<U> result(rows_, cols_);
    std::transform(mem_.begin(), mem_.end(), result.mem_.begin(), f);
    return result;
  }

  template <std::invocable<const T&> F>
  Matrix<T>& transform(F&& f) {
    std::transform(mem_.begin(), mem_.end(), mem_.begin(), std::forward(f));
    return *this;
  }

  Matrix operator+(const Matrix& rhs) const {
    if (rows_ != rhs.rows_ || cols_ != rhs.cols_) {
      throw std::invalid_argument("matrix sizes are not the same");
    }

    Matrix result(rows_, cols_);
    for (int i = 0; i < rows_ * cols_; i++) {
      result.mem_[i] = mem_[i] + rhs.mem_[i];
    }
    return result;
  }

  Matrix& operator+=(const Matrix& rhs) {
    *this = std::move(*this + rhs);
    return *this;
  }

  Matrix operator+(const T& scalar) const {
    return map([&scalar](const T& value) { return value + scalar; });
  }

  Matrix& operator+=(const T& scalar) {
    return transform([&scalar](const T& value) { return value + scalar; });
  }

  Matrix operator*(const Matrix& rhs) const {
    if (cols_ != rhs.rows_) {
      throw std::invalid_argument(
          "matrix sizes are not compatible for multiplication");
    }

    Matrix result(rows_, rhs.cols_);
    for (int i = 0; i < rows_; i++) {
      for (int j = 0; j < rhs.cols_; j++) {
        result[i][j] = T{};
        for (int k = 0; k < cols_; k++) {
          result[i][j] += (*this)[i][k] * rhs[k][j];
        }
      }
    }
    return result;
  }

  Matrix& operator*=(const Matrix& rhs) {
    *this = std::move(*this * rhs);
    return *this;
  }

  Matrix operator*(const T& scalar) const {
    return map([&scalar](const T& value) { return value * scalar; });
  }

  Matrix& operator*=(const T& scalar) {
    return transform([&scalar](const T& value) { return value * scalar; });
  }
};
