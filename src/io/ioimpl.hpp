#pragma once
#include "iobuf.hpp"
#include "src/io/io.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>

namespace io {
class StringReaderWriter : public ReaderWriter {
  std::string data_;
  size_t read_pos_ = 0;
  size_t write_pos_ = 0;

public:
  StringReaderWriter() = default;
  StringReaderWriter(std::string&& str) : data_(std::move(str)) {}

  Expected<size_t> read(std::span<std::byte> buf) override {
    if (read_pos_ == data_.size())
      return Unexpected(Err::UnexpectedEof);
    size_t n = std::min(buf.size(), data_.size() - read_pos_);
    std::copy_n(reinterpret_cast<std::byte*>(data_.data()) + read_pos_, n,
                buf.begin());
    read_pos_ += n;
    return n;
  }

  Expected<size_t> write(std::span<const std::byte> buf) override {
    size_t n = buf.size();
    if (write_pos_ + n > data_.size())
      data_.resize(write_pos_ + n);
    std::memcpy(data_.data() + write_pos_, buf.data(), n);
    write_pos_ += n;
    return n;
  }

  Expected<void> flush() override { return {}; }
};

class StringBufReaderWriter : public BufReaderWriter {
  StringReaderWriter inner_;

public:
  StringBufReaderWriter(std::string&& str)
      : BufReaderWriter(inner_), inner_(std::move(str)) {}
};

class FileReaderWriter : public ReaderWriter {
  FILE* file_;

public:
  FileReaderWriter(FILE* file) : file_(file) {}

  Expected<size_t> read(std::span<std::byte> buf) override {
    size_t n = std::fread(buf.data(), 1, buf.size(), file_);
    if (n == 0 && std::ferror(file_))
      return Unexpected(Err::InvalidData);
    return n;
  }

  Expected<size_t> write(std::span<const std::byte> buf) override {
    size_t n = std::fwrite(buf.data(), 1, buf.size(), file_);
    if (n < buf.size() && std::ferror(file_))
      return Unexpected(Err::InvalidData);
    return n;
  }

  Expected<void> flush() override { return {}; }
};

class FileBufReaderWriter : public BufReaderWriter {
  FileReaderWriter inner_;

public:
  FileBufReaderWriter(FILE* file) : BufReaderWriter(inner_), inner_(file) {}
};

} // namespace io
