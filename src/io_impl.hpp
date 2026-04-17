#pragma once

#include "../src/io.hpp"
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace io {
class StringReaderWriter : public ReaderWriter {
  std::string str_;

public:
  StringReaderWriter() : str_("") {};
  StringReaderWriter(auto&& str) : str_(std::forward<decltype(str)>(str)) {};

  Expected<size_t> read(std::span<std::byte> buf) override {
    if (str_.empty()) {
      return Unexpected(Err::UnexpectedEof);
    }
    
    size_t number_of_bytes = std::min(buf.size(), str_.size());
    
    auto read = std::as_bytes(std::span(str_.data(), number_of_bytes));
    std::ranges::copy(read, buf.begin());
    str_ = str_.substr(number_of_bytes);
    return {number_of_bytes};
  }

  Expected<size_t> write(std::span<const std::byte> buf) override {
    str_.append(std::string_view(reinterpret_cast<const char*>(buf.data()),
                                 buf.size()));
    return {buf.size()};
  }
};

class FileReaderWriter : public ReaderWriter {};

}; // namespace io
