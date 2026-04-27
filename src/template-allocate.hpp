#include <concepts>
#include <cstddef>
#include <type_traits>

template <size_t SIZE, typename... Types>
  requires(SIZE >= (sizeof(std::remove_reference_t<Types>) + ...)) &&
          (std::copy_constructible<std::remove_reference_t<Types>> && ...)
void allocate(void* m, Types... a) {
  ((new (m) std::remove_reference_t<decltype(a)>(a),
    m = static_cast<char*>(m) + sizeof(std::remove_reference_t<decltype(a)>)),
   ...);
}
