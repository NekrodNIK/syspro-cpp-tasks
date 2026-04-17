#pragma once
#include "io.hpp"
#include <algorithm>
#include <cstring>
#include <vector>

namespace io {

class BufReader : public Reader {
  Reader& inner_;
  std::vector<std::byte> buf_;
  size_t pos_ = 0;
  size_t len_ = 0;

  Expected<void> fill_buf_() {
    pos_ = 0;
    auto res = inner_.read(buf_);
    if (!res)
      return Unexpected(res.error());
    len_ = res.value();
    return {};
  }

public:
  BufReader(Reader& inner, size_t buf_size = 4096)
      : inner_(inner), buf_(buf_size) {}

  Expected<size_t> read(std::span<std::byte> buf) override {
    size_t total = 0;
    while (!buf.empty()) {
      if (pos_ == len_) {
        auto res = fill_buf_();
        if (!res)
          return Unexpected(res.error());
        if (len_ == 0)
          break;
      }
      size_t n = std::min(buf.size(), len_ - pos_);
      std::copy_n(buf_.begin() + pos_, n, buf.data());
      pos_ += n;
      buf = buf.subspan(n);
      total += n;
    }
    return total;
  }
};

class BufWriter : public Writer {
  Writer& inner_;
  std::vector<std::byte> buf_;
  size_t pos_ = 0;

public:
  BufWriter(Writer& inner, size_t buf_size = 4096)
      : inner_(inner), buf_(buf_size) {}

  ~BufWriter() { flush(); }

  Expected<size_t> write(std::span<const std::byte> buf) override {
    size_t total = 0;
    while (!buf.empty()) {
      if (pos_ == buf_.size()) {
        auto res = flush();
        if (!res)
          return Unexpected(res.error());
      }
      size_t n = std::min(buf.size(), buf_.size() - pos_);
      std::copy_n(buf.data(), n, buf_.data() + pos_);
      pos_ += n;
      buf = buf.subspan(n);
      total += n;
    }
    return total;
  }

  Expected<void> flush() override {
    if (pos_ == 0)
      return {};
    auto res = inner_.write_all(std::span(buf_.data(), pos_));
    if (!res)
      return res;
    pos_ = 0;
    return {};
  }
};

class BufReaderWriter : public BufReader, public BufWriter {
public:
  BufReaderWriter(ReaderWriter& inner, size_t read_buf_size = 4096,
                  size_t write_buf_size = 4096)
      : BufReader(inner, read_buf_size), BufWriter(inner, write_buf_size) {}
};
} // namespace io
