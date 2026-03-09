#pragma once
#include <algorithm>
#include <utility>
template <typename T>
class ScopedPtr {
protected:
  T* raw;

public:
  ScopedPtr() : raw(nullptr) {};
  ScopedPtr(T* raw) : raw(raw) {};
  virtual ~ScopedPtr() { delete raw; }
  T* get() { return raw; }
  const T* get() const { return raw; }
};

template <typename T>
class CloningScopedPtr : public ScopedPtr<T> {
public:
  CloningScopedPtr& operator=(const ScopedPtr<T>& other) {
    T* ptr = other.raw ? new T(*other.raw) : nullptr;
    if (this->raw)
      delete this->raw;
    this->raw = ptr;
    return *this;
  }
  CloningScopedPtr(const ScopedPtr<T>& other) { this->operator=(other); }

  CloningScopedPtr(ScopedPtr<T>&& other) = delete;
  CloningScopedPtr& operator=(ScopedPtr<T>&& other) = delete;
};

template <typename T>
class UniquePtr : public ScopedPtr<T> {
public:
  UniquePtr& operator=(ScopedPtr<T>&& other) {
    T* ptr = std::exchange(other.raw, nullptr);
    if (this->raw)
      delete this->raw;
    this->raw = ptr;
    return *this;
  }
  UniquePtr(ScopedPtr<T>&& other) { this->operator=(std::move(other)); }

  UniquePtr(const ScopedPtr<T>&) = delete;
  UniquePtr& operator=(const ScopedPtr<T>&) = delete;
};
