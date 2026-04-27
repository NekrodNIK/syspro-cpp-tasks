#include <utility>
template <typename Checker, typename... Args>
int getIndexOfFirstMatch(Checker&& check, Args&&... args) {
  int i = 0;
  return ((check(std::forward<Args>(args)) || !(++i)) || ...) ? i : -1;
}
