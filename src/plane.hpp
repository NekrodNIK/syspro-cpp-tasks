#pragma once
#include <cassert>
#include <cmath>
#include <optional>

namespace plane {
bool partial_eq(double a, double b) { return std::abs(a - b) < 1e-9; }

struct Vector {
  double x, y;

  static Vector zero() { return {0, 0}; };
  bool operator==(const Vector& v) const {
    return partial_eq(x, v.x) && partial_eq(y, v.y);
  }
  Vector operator+(const Vector& v) const { return {x + v.x, y + v.y}; }
  Vector operator-(const Vector& v) const { return {x - v.x, y - v.y}; }
  double operator*(const Vector& v) const { return x * v.x + y * v.y; }
  Vector ortogonal() const { return Vector(y, -x); }
  bool isOrtogonal(const Vector& v) const { return partial_eq(*this * v, 0); }
  bool isCollinear(const Vector& v) const {
    return partial_eq(x * v.y, y * v.x);
  }
};

struct Point : Vector {
  auto operator*(auto) = delete;
  auto ortogonal() = delete;
  auto isOrtogonal() = delete;
  auto isCollinear() = delete;
};

struct Line {
  Point start;
  Vector direction;

  Line(const Point& a, const Point& b) : start(a), direction(b - a) {
    assert(direction != Vector::zero());
  }
  Line(const Point& start, const Vector& direction)
      : start(start), direction(direction) {};
  std::optional<Point> intersection(const Line& other) const {
    if (direction.isCollinear(other.direction)) 
      return std::nullopt;

    double x1 = start.x, y1 = start.y, dx1 = direction.x, dy1 = direction.y,
           x2 = other.start.x, y2 = other.start.y, dx2 = other.direction.x,
           dy2 = other.direction.y;
    double det = dx1 * dy2 - dy1 * dx2;
    if (partial_eq(det, 0)) 
      return std::nullopt;

    double t = ((x2 - x1) * dy2 - (y2 - y1) * dx2) / det;
    return std::optional<Point>({x1 + dx1 * t, y1 + dy1 * t});
  }
  Line perpendicular(const Point& point) const {
    return Line(point, direction.ortogonal());
  }
};

} // namespace plane
