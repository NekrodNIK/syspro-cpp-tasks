#include <cstddef>
#include <exception>

class limit_error : public std::exception {
public:
  const char* what() const noexcept override { return "limit"; }
};

template <typename D, size_t max>
struct InstanceLimiterMixin {
  static inline size_t cnt = 0;
  InstanceLimiterMixin() { if (cnt < max) cnt++; else throw limit_error(); }
  ~InstanceLimiterMixin() { cnt--; }
  InstanceLimiterMixin(const InstanceLimiterMixin&) { if (cnt < max) cnt++; else throw limit_error(); }
};

template <typename D>
struct SingletonLimiterMixin : public InstanceLimiterMixin<D, 1> {};
