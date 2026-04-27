template <typename Checker, typename... Args>
int getIndexOfFirstMatch(Checker&& check, Args&&... args) {
  int i = 0;
  return ((check(args) || !(++i)) || ...) ? i : -1;
}

