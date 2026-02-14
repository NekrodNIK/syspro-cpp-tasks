#pragma once
#include <algorithm>
#include <cassert>
#include <concepts>
#include <memory>
#include <utility>

namespace cpp_utils {
template <typename T>
  requires std::totally_ordered<T>
class AvlTreeSet {
  struct Node {
    T value;
    int height;

    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    Node* parent;

    Node() {};
    Node(T value) : value(value), height(1) {};

    int lh() { return this->left ? this->left->height : 0; }
    int rh() { return this->right ? this->right->height : 0; }
    void updateHeight() { this->height = std::max(this->lh(), this->rh()) + 1; }
    int balance() { return this->rh() - this->lh(); }
  };

  std::unique_ptr<Node> header = std::make_unique<Node>();
  std::unique_ptr<Node>& root = header->left;
  Node* leftmost;

  void setLeft(Node* node, std::unique_ptr<Node> new_left) {
    node->left = std::move(new_left);
    if (node->left)
      node->left->parent = node;
    node->updateHeight();
  }

  void setRight(Node* node, std::unique_ptr<Node> new_right) {
    node->right = std::move(new_right);
    if (node->right)
      node->right->parent = node;
    node->updateHeight();
  }

  void rotateL(std::unique_ptr<Node>& node) {
    assert(node && node->right);

    auto pivot = std::move(node->right);
    setRight(node.get(), std::move(pivot->left));
    setLeft(pivot.get(), std::move(node));

    node = std::move(pivot);
  }

  void rotateR(std::unique_ptr<Node>& node) {
    assert(node && node->left);

    auto pivot = std::move(node->left);
    setLeft(node.get(), std::move(pivot->right));
    setRight(pivot.get(), std::move(node));

    node = std::move(pivot);
  }

  void balanceTree(std::unique_ptr<Node>& node) {
    switch (node->balance()) {
    case 2:
      if (node->right->balance() == -1) {
        rotateR(node->right);
      }
      rotateL(node);
      break;
    case -2:
      if (node->left->balance() == 1) {
        rotateL(node->left);
      }
      rotateR(node);
      break;
    }
    node->updateHeight();
  }

  void updateAncestors(Node* child) {
    auto cur = child->parent;

    while (auto next = cur->parent) {
      balanceTree(next->left.get() == cur ? next->left : next->right);
      cur = next;
    }
  }

  void updateLeftmost() {
    leftmost = header.get();
    while (leftmost->left) {
      leftmost = leftmost->left.get();
    }
  }

public:
  class iterator {
    Node* node;

    void next() {
      if (node->right) {
        node = node->right.get();
        while (node->left) {
          node = node->left.get();
        }
      } else {
        auto parent = node->parent;
        while (node == parent->right) {
          node = parent;
          parent = parent->parent;
        }
        node = parent;
      }
    }

    void prev() {
      if (node->left) {
        node = node->left.get();
        while (node->right) {
          node = node->right.get();
        }
      } else {
        auto parent = node->parent;
        while (parent && node == parent->left) {
          node = parent;
          parent = parent->parent;
        }

        if (parent)
          node = parent;
      }
    }

    iterator(Node* node) { this->node = node; }
    friend class AvlTreeSet<T>;

  public:
    bool operator==(iterator other) { return node == other.node; }
    T& operator*() { return node->value; }
    T* operator->() { return &node->value; }

    iterator& operator++() {
      next();
      return *this;
    }

    iterator operator++(int) {
      auto saved = iterator(node);
      next();
      return saved;
    }

    iterator& operator--() {
      prev();
      return *this;
    }

    iterator operator--(int) {
      auto saved = iterator(node);
      prev();
      return saved;
    }
  };

  iterator begin() { return iterator(leftmost); };
  iterator end() { return iterator(header.get()); }

  iterator find(T value) {
    auto ptr = root;
    while (ptr) {
      if (ptr->value == value) {
        return iterator(ptr);
      } else if (ptr->value < value) {
        ptr = ptr->right;
      } else {
        ptr = ptr->left;
      }
    }

    return end();
  }

  iterator upper_bound(const T& value) {
    auto result = header;

    auto ptr = root;
    while (ptr) {
      if (ptr->value <= value) {
        ptr = ptr->right;
      } else {
        result = ptr;
        ptr = ptr->left;
      }
    }

    return iterator(result);
  }

  void insert(T value) {
    auto new_node = std::make_unique<Node>(value);

    if (begin() == end()) {
      root = std::move(new_node);
      root->parent = header.get();
      updateLeftmost();
    }

    Node* prev;
    Node* cur = root.get();
    while (cur) {
      prev = cur;
      cur = ((value < cur->value) ? cur->left : cur->right).get();
    }

    if (value < prev->value) {
      setLeft(prev, std::move(new_node));
    } else {
      setRight(prev, std::move(new_node));
    }

    updateAncestors(prev);
    updateLeftmost();
  }

  void remove(const iterator& iter) {
    if (iter == end()) {
      return;
    }

    auto rm = iter.node;
    std::unique_ptr<Node> replacement;

    if (rm->left && rm->right) {
      auto succ = rm->right.get();
      auto parent = rm;

      while (succ->left) {
        parent = succ;
        succ = succ->left.get();
      }

      if (parent != rm) {
        setLeft(parent, std::move(succ->right));
        setRight(succ, std::move(rm->right));
      }

      setLeft(succ, rm->left);
      replacement = succ;
    } else {
      replacement = rm->left ? rm->left : rm->right;
    }

    auto parent = rm->parent;
    if (parent->left == rm) {
      setLeft(parent, replacement);
    } else {
      setRight(parent, replacement);
    }

    updateAncestors(parent);
    updateLeftmost();
  }
};
} // namespace cpp_utils
