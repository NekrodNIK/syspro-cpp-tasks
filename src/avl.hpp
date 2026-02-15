#pragma once
#include <cassert>
#include <compare>
#include <concepts>
#include <csignal>
#include <functional>
#include <memory>

namespace cpp_utils {
template <typename T> requires std::totally_ordered<T>
class AvlTreeSet {
  struct Node {
    T value;
    int size = 1;
    int height = 1;

    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    std::weak_ptr<Node> parent;

    int rs() { return right ? right->size : 0; }
    int ls() { return left ? left->size : 0; }
    int rh() { return right ? right->height : 0; }
    int lh() { return left ? left->height : 0; }
    void recalc() {
      size = rs() + ls() + 1;
      height = std::max(rh(), lh()) + 1;
    }
    int getBalance() { return rh() - lh(); }
  };

  static void setLeft(std::shared_ptr<Node> parent,
                      std::shared_ptr<Node> left) {
    if (left) {
      left->parent = parent;
    }
    parent->left = left;
    parent->recalc();
  }

  static void setRight(std::shared_ptr<Node> parent,
                       std::shared_ptr<Node> right) {
    if (right) {
      right->parent = parent;
    }
    parent->right = right;
    parent->recalc();
  }

  std::shared_ptr<Node> header = std::make_shared<Node>();
  std::shared_ptr<Node>& bst_root = header->left;
  std::shared_ptr<Node> leftmost = header;

  std::shared_ptr<Node> rotateL(std::shared_ptr<Node> root) {
    assert(root && root->right);
    auto pivot = root->right;

    setRight(root, pivot->left);
    setLeft(pivot, root);

    return pivot;
  }

  std::shared_ptr<Node> rotateR(std::shared_ptr<Node> root) {
    assert(root && root->left);
    auto pivot = root->left;

    setLeft(root, pivot->right);
    setRight(pivot, root);

    return pivot;
  }

  std::shared_ptr<Node> balanceTree(std::shared_ptr<Node> root) {
    if (root->getBalance() == 2) {
      if (root->right->getBalance() == -1) {
        setRight(root, rotateR(root->right));
      }
      root = rotateL(root);
    } else if (root->getBalance() == -2) {
      if (root->left->getBalance() == 1) {
        setLeft(root, rotateL(root->left));
      }
      root = rotateR(root);
    }

    return root;
  }

  void updateAncestors(std::shared_ptr<Node> start) {
    auto cur = start;
    cur->recalc();

    while (auto parent = cur->parent.lock()) {
      auto& ref = (parent->left == cur) ? parent->left : parent->right;
      ref = balanceTree(cur);
      ref->parent = parent;

      cur = parent;
    }
  }

  void updateLeftmost() {
    leftmost = header;
    while (leftmost->left) {
      leftmost = leftmost->left;
    }
  }

public:
  class iterator {
    friend class AvlTree;
    std::shared_ptr<Node> node;

    void next() {
      if (node->right) {
        node = node->right;
        while (node->left) {
          node = node->left;
        }
      } else {
        auto parent = node->parent.lock();
        while (node == parent->right) {
          node = parent;
          parent = parent->parent.lock();
        }
        node = parent;
      }
    }

    void prev() {
      if (node->left) {
        node = node->left;
        while (node->right) {
          node = node->right;
        }
      } else {
        auto parent = node->parent.lock();
        while (parent && node == parent->left) {
          node = parent;
          parent = parent->parent.lock();
        }

        if (parent) {
          node = parent;
        }
      }
    }

    friend AvlTreeSet<T>;

  public:
    iterator(std::shared_ptr<Node> node) { this->node = node; }
    bool operator==(iterator other) const { return node == other.node; }
    bool operator!=(iterator other) const { return node != other.node; }
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

  int rank(const iterator& it) {
    if (it == end())
      return 0;
    auto node = it.node;

    int rank = node->rs();
    auto cur = node;

    std::shared_ptr<Node> parent;
    while ((parent = cur->parent.lock()) != header) {
      if (cur == parent->left) {
        rank += parent->rs() + 1;
      }
      cur = parent;
    }

    return rank;
  }

  iterator begin() { return iterator(leftmost); };
  iterator end() { return iterator(header); }

  iterator find(T value) {
    auto ptr = bst_root;
    while (ptr) {
      auto comp_result = ptr->value <=> value;
      if (comp_result == 0) {
        return iterator(ptr);
      } else if (comp_result < 0) {
        ptr = ptr->right;
      } else {
        ptr = ptr->left;
      }
    }

    return end();
  }

  iterator upperBound(const T& value) {
    auto result = header;

    auto ptr = bst_root;
    while (ptr) {
      auto comp_result = ptr->value <=> value;
      if (comp_result <= 0) {
        ptr = ptr->right;
      } else {
        result = ptr;
        ptr = ptr->left;
      }
    }

    return iterator(result);
  }

  iterator insert(T value) {
    auto new_node = std::make_shared<Node>(value);

    if (begin() == end()) {
      bst_root = new_node;
      bst_root->parent = header;
      updateLeftmost();
      return iterator(new_node);
    }

    std::shared_ptr<Node> prev;
    auto cur = bst_root;
    while (cur) {
      prev = cur;
      cur = (value < cur->value) ? cur->left : cur->right;
    }

    if (value == prev->value) {
      return iterator(prev);
    }

    auto& ref = (value < prev->value) ? prev->left : prev->right;
    ref = new_node;
    new_node->parent = prev;

    updateAncestors(prev);
    updateLeftmost();
    return iterator(new_node);
  }

  void remove(const iterator& iter) {
    if (iter == end()) {
      return;
    }

    auto rm = iter.node;
    std::shared_ptr<Node> replacement;

    if (rm->left && rm->right) {
      auto succ = rm->right;
      auto parent = rm;

      while (succ->left) {
        parent = succ;
        succ = succ->left;
      }

      if (parent != rm) {
        setLeft(parent, succ->right);
        setRight(succ, rm->right);
      }

      setLeft(succ, rm->left);
      replacement = succ;
    } else {
      replacement = rm->left ? rm->left : rm->right;
    }

    auto parent = rm->parent.lock();
    auto& ref = (parent->left == rm) ? parent->left : parent->right;
    ref = replacement;
    if (ref) {
      ref->parent = parent;
    }

    updateAncestors(parent);
    updateLeftmost();
  }
};
}
