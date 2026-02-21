#include <algorithm>
#include <concepts>
#include <memory>
#include <utility>

namespace lib {
template <typename T>
struct AvlNode {
  T value;

  int height;
  std::unique_ptr<AvlNode> left, right;
  AvlNode* parent;

  AvlNode(T value)
      : value(value), height(1), left(nullptr), right(nullptr),
        parent(nullptr) {}
  AvlNode() : left(nullptr) {};
  AvlNode(AvlNode&&) = default;
  AvlNode& operator=(AvlNode&&) = default;

  int get_balance() const;
  void update_height();

  void set_left(std::unique_ptr<AvlNode>);
  void set_right(std::unique_ptr<AvlNode>);

  static std::unique_ptr<AvlNode> rotate_left(std::unique_ptr<AvlNode>);
  static std::unique_ptr<AvlNode> rotate_right(std::unique_ptr<AvlNode>);
  static std::unique_ptr<AvlNode> balance_tree(std::unique_ptr<AvlNode>);
};

template <std::totally_ordered T>
class AvlOrderedSet {
  std::unique_ptr<AvlNode<T>> header_;
  AvlNode<T>* leftmost_;

  void balance_ancestors_(AvlNode<T>&);
  void update_leftmost_();

public:
  class iterator {
    friend class AvlOrderedSet<T>;

    AvlNode<T>* node;
    iterator(AvlNode<T>* node) : node(node) {}

  public:
    iterator() = delete;
    bool operator==(const iterator&) const = default;
    bool operator!=(const iterator&) const = default;
    T& operator*() { return node->value; }
    T* operator->() { return &node->value; }

    iterator& operator++();
    iterator operator++(int) {
      auto prev = iterator(node);
      ++*this;
      return prev;
    };
    iterator& operator--();
    iterator operator--(int) {
      auto prev = iterator(node);
      --*this;
      return prev;
    };
  };

  AvlOrderedSet();
  AvlOrderedSet(const AvlOrderedSet&);
  AvlOrderedSet& operator=(const AvlOrderedSet&);
  AvlOrderedSet(AvlOrderedSet&&);
  AvlOrderedSet& operator=(AvlOrderedSet&&);

  iterator begin() const { return iterator(leftmost_); };
  iterator end() const { return iterator(header_.get()); };
  iterator find(const T&) const;
  iterator upper_bound(const T&) const;
  void insert(T);
  void remove(const T&);
};

template <typename T>
int AvlNode<T>::get_balance() const {
  return (right ? right->height : 0) - (left ? left->height : 0);
}

template <typename T>
void AvlNode<T>::update_height() {
  height = std::max(right ? right->height : 0, left ? left->height : 0) + 1;
}

template <typename T>
void AvlNode<T>::set_left(std::unique_ptr<AvlNode<T>> left) {
  this->left = std::move(left);
  if (this->left)
    this->left->parent = this;
  this->update_height();
}

template <typename T>
void AvlNode<T>::set_right(std::unique_ptr<AvlNode<T>> right) {
  this->right = std::move(right);
  if (this->right)
    this->right->parent = this;
  this->update_height();
}

template <typename T>
std::unique_ptr<AvlNode<T>>
AvlNode<T>::rotate_left(std::unique_ptr<AvlNode<T>> node) {
  auto pivot = std::move(node->right);
  node->set_right(std::move(pivot->left));
  pivot->set_left(std::move(node));
  return pivot;
}

template <typename T>
std::unique_ptr<AvlNode<T>>
AvlNode<T>::rotate_right(std::unique_ptr<AvlNode<T>> node) {
  auto pivot = std::move(node->left);
  node->set_left(std::move(pivot->right));
  pivot->set_right(std::move(node));
  return pivot;
}

template <typename T>
std::unique_ptr<AvlNode<T>>
AvlNode<T>::balance_tree(std::unique_ptr<AvlNode<T>> node) {
  if (!node) {
    return node;
  }

  if (node->get_balance() == 2) {
    if (node->right->get_balance() == -1) {
      node->right = rotate_right(std::move(node->right));
      node->right->parent = node.get();
    }
    return rotate_left(std::move(node));
  } else if (node->get_balance() == -2) {
    if (node->left->get_balance() == 1) {
      node->left = rotate_left(std::move(node->left));
      node->left->parent = node.get();
    }
    return rotate_right(std::move(node));
  }
  return node;
}

template <std::totally_ordered T>
AvlOrderedSet<T>::iterator& AvlOrderedSet<T>::iterator::operator++() {
  if (node->right) {
    node = node->right.get();
    while (node->left) {
      node = node->left.get();
    }
  } else {
    while (node->parent && node == node->parent->right.get()) {
      node = node->parent;
    }
    node = node->parent;
  }
  return *this;
}

template <std::totally_ordered T>
AvlOrderedSet<T>::iterator& AvlOrderedSet<T>::iterator::operator--() {
  if (node->left) {
    node = node->left.get();
    while (node->right) {
      node = node->right.get();
    }
  } else {
    while (node->parent && node == node->parent->left.get()) {
      node = node->parent;
    };
    node = node->parent;
  }
  return *this;
}

template <std::totally_ordered T>
AvlOrderedSet<T>::AvlOrderedSet() {
  this->header_ = std::make_unique<AvlNode<T>>();
  this->leftmost_ = this->header_.get();
}

template <std::totally_ordered T>
AvlOrderedSet<T>::AvlOrderedSet(const AvlOrderedSet<T>& other) {
  *this = other;
}

template <std::totally_ordered T>
AvlOrderedSet<T>& AvlOrderedSet<T>::operator=(const AvlOrderedSet<T>& other) {
  auto deep_copy = [](this const auto& self,
                      const AvlNode<T>* node) -> std::unique_ptr<AvlNode<T>> {
    if (!node)
      return nullptr;
    auto copy = std::make_unique<AvlNode<T>>();

    copy->value = node->value;
    copy->height = node->height;
    copy->left = self(node->left.get());
    copy->right = self(node->right.get());
    if (copy->left)
      copy->left->parent = copy.get();
    if (copy->right)
      copy->right->parent = copy.get();
    return copy;
  };
  header_ = deep_copy(other.header_.get());
  leftmost_ = header_.get();
  return *this;
}

template <std::totally_ordered T>
AvlOrderedSet<T>::AvlOrderedSet(AvlOrderedSet<T>&& other) {
  *this = std::move(other);
}

template <std::totally_ordered T>
AvlOrderedSet<T>& AvlOrderedSet<T>::operator=(AvlOrderedSet<T>&& other) {
  header_ = std::move(other.header_);
  other.header_ = std::make_unique<AvlNode<T>>();
  leftmost_ = header_.get();
  other.leftmost_ = other.header_.get();
  return *this;
}

template <std::totally_ordered T>
AvlOrderedSet<T>::iterator AvlOrderedSet<T>::find(const T& value) const {
  AvlNode<T>* current = header_->left.get();
  while (current) {
    if (current->value == value) {
      return iterator(current);
    } else if (current->value > value) {
      current = current->left.get();
    } else {
      current = current->right.get();
    }
  }
  return end();
}

template <std::totally_ordered T>
AvlOrderedSet<T>::iterator AvlOrderedSet<T>::upper_bound(const T& value) const {
  iterator result = end();

  AvlNode<T>* current = header_->left.get();
  while (current) {
    if (current->value <= value) {
      current = current->right.get();
    } else {
      result = iterator(current);
      current = current->left.get();
    }
  }

  return result;
}

template <std::totally_ordered T>
void AvlOrderedSet<T>::balance_ancestors_(AvlNode<T>& node) {
  AvlNode<T>* current = node.parent;

  while (current != header_.get()) {
    auto& child =
        current->left.get() == node.parent ? current->left : current->right;
    child = AvlNode<T>::balance_tree(std::move(child));
    if (child)
      child->parent = current;
    current = current->parent;
  }
}

template <std::totally_ordered T>
void AvlOrderedSet<T>::update_leftmost_() {
  leftmost_ = header_.get();
  while (leftmost_->left) {
    leftmost_ = leftmost_->left.get();
  }
}

template <std::totally_ordered T>
void AvlOrderedSet<T>::insert(T value) {
  std::unique_ptr<AvlNode<T>>* current = &header_->left;
  AvlNode<T>* parent = header_.get();

  while (*current) {
    if ((*current)->value == value) {
      return;
    }
    parent = (*current).get();
    if ((*current)->value > value) {
      current = &(*current)->left;
    } else {
      current = &(*current)->right;
    }
  }

  *current = std::make_unique<AvlNode<T>>(AvlNode(value));
  (*current)->parent = parent;
  parent->update_height();
  balance_ancestors_(*(*current).get());
  update_leftmost_();
}

template <std::totally_ordered T>
void AvlOrderedSet<T>::remove(const T& value) {
  auto found = find(value);
  if (found == end()) {
    return;
  }

  auto node = found.node;
  auto& rm = (node->parent->left.get() == node) ? node->parent->left
                                                : node->parent->right;
  auto replacement = std::unique_ptr<AvlNode<T>>(nullptr);

  if (rm->left && rm->right) {
    auto* succ = &rm->right;
    auto* parent = &rm;

    while ((*succ)->left) {
      parent = succ;
      succ = &(*succ)->left;
    }

    if (*parent != rm) {
      (*parent)->set_left(std::move((*succ)->right));
      (*succ)->set_right(std::move(rm->right));
    }

    (*succ)->set_left(std::move(rm->left));
    replacement = std::move(*succ);
  } else {
    replacement = std::move(rm->left ? rm->left : rm->right);
  }

  if (replacement) {
    replacement->parent = rm->parent;
  }
  rm = std::move(replacement);

  if (rm) {
    balance_ancestors_(*rm.get());
  }
  update_leftmost_();
}
} // namespace lib
