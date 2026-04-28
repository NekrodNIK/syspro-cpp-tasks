#include <any>
#include <cstddef>
#include <stdexcept>
#include <utility>

template <size_t Ind, typename T, typename... Tail>
struct type_at {
  using type = type_at<Ind - 1, Tail...>::type;
};

template <typename T, typename... Tail>
struct type_at<0, T, Tail...> {
  using type = T;
};

template <size_t Ind, typename T>
struct container_leaf {
  T value;
};

template <typename Seq, typename... Types>
struct container_inner;
template <size_t... I, typename... Types>
struct container_inner<std::index_sequence<I...>, Types...>
    : container_leaf<I, Types>... {
  container_inner(Types... values) : container_leaf<I, Types>(values)... {}
};

template <typename... Types>
class container
    : container_inner<std::index_sequence_for<Types...>, Types...> {

  template <size_t Ind, typename T, typename... Tail>
  std::any getElementImpl(size_t ind) {
    if (ind == Ind)
      return dynamic_cast<container_leaf<Ind, T>*>(this)->value;
    if constexpr (sizeof...(Tail) > 0)
      return getElementImpl<Ind + 1, Tail...>(ind);
    else
      throw std::out_of_range("Index not found");
  }

public:
  container(Types... values)
      : container_inner<std::index_sequence_for<Types...>, Types...>(
            values...) {};

  std::any getElement(size_t ind) {
    if (ind >= sizeof...(Types))
      throw std::out_of_range("Index not found");
    return getElementImpl<0, Types...>(ind);
  }

  template <typename T>
  T getElement(size_t ind) {
    return std::any_cast<T>(getElement(ind));
  }

  template <size_t ind, typename T = type_at<ind, Types...>::type>
  T getElement() {
    return dynamic_cast<container_leaf<ind, T>*>(this)->value;
  }
};
