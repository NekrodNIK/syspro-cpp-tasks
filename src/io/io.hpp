#pragma once
#include <cctype>
#include <charconv>
#include <concepts>
#include <cstddef>
#include <expected>
#include <format>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <utility>

namespace io {
enum class Err : unsigned char {
  UnexpectedEof,
  WriteZero,
  InvalidData,
};

template <typename T>
using Expected = std::expected<T, Err>;
using Unexpected = std::unexpected<Err>;

class Reader;
class Writer;
class ReaderWriter;
template <typename T>
class ReadInto;
template <typename T>
class WriteFrom;

class Reader {
public:
  virtual Expected<size_t> read(std::span<std::byte> buf) = 0;

  virtual ~Reader() = default;
  virtual Expected<void> read_exact(std::span<std::byte> buf);
  template <typename T>
  Expected<void> read_into(T& dest) {
    return ReadInto<T>::read_into(*this, dest);
  };

  class OperatorWrapper {
    friend class Reader;
    Expected<void> error;
    Reader& r;
    OperatorWrapper(Expected<void> error, Reader& r) : error(error), r(r) {};

  public:
    operator Expected<void>() { return error; }
    template <typename T>
    OperatorWrapper operator>>(T& dest) {
      if (!error.has_value())
        return *this;
      return OperatorWrapper(r.read_into(dest), r);
    }
  };

  template <typename T>
  OperatorWrapper operator>>(T& dest) {
    return OperatorWrapper(read_into(dest), *this);
  };
};

class Writer {
public:
  virtual Expected<size_t> write(std::span<const std::byte> buf) = 0;
  virtual Expected<void> flush() = 0;

  virtual ~Writer() = default;
  virtual Expected<void> write_all(std::span<const std::byte> buf);

  template <typename... Args>
  std::expected<void, Err> write_fmt(std::format_string<Args...> fmt,
                                     Args&&... args);

  template <typename T>
  Expected<void> write_from(T&& src) {
    return WriteFrom<std::remove_reference_t<T>>::write_from(*this, src);
  };

  class OperatorWrapper {
    friend class Writer;
    Expected<void> error;
    Writer& w;
    OperatorWrapper(Expected<void> error, Writer& w) : error(error), w(w) {};

  public:
    operator Expected<void>() { return error; }
    template <typename T>
    OperatorWrapper operator<<(T&& src) {
      if (!error.has_value())
        return *this;
      return OperatorWrapper(w.write_from(std::forward<T>(src)), w);
    }
  };

  template <typename T>
  OperatorWrapper operator<<(T&& src) {
    return OperatorWrapper(write_from(std::forward<T>(src)), *this);
  }
};

class ReaderWriter : public Reader, public Writer {};

template <typename T>
class ReadInto {
public:
  static Expected<void> read_into(Reader& r, T& dest);
};

template <typename T>
class WriteFrom {
public:
  static Expected<void> write_from(Writer& w, const T& src);
};

inline Expected<void> Reader::read_exact(std::span<std::byte> buf) {
  while (!buf.empty()) {
    auto res = read(buf);
    if (!res)
      return Unexpected(res.error());
    if (*res == 0)
      break;
    buf = buf.subspan(res.value());
  }
  if (!buf.empty())
    return Unexpected(Err::UnexpectedEof);
  return {};
}

inline Expected<void> Writer::write_all(std::span<const std::byte> buf) {
  while (!buf.empty()) {
    auto res = write(buf);
    if (!res)
      return Unexpected(res.error());
    if (*res == 0)
      return Unexpected(Err::WriteZero);
    buf = buf.subspan(res.value());
  }
  return {};
}

template <typename... Args>
std::expected<void, Err> Writer::write_fmt(std::format_string<Args...> fmt,
                                           Args&&... args) {
  return write_from(std::format(fmt, std::forward<Args>(args)...));
}

template <>
inline Expected<void> ReadInto<std::byte>::read_into(Reader& r,
                                                     std::byte& dest) {
  return r.read_exact(std::span(&dest, 1));
}

inline Expected<char> read_first_non_space_char(Reader& r) {
  std::byte b;
  char ch;
  do {
    Expected<void> res;
    if (!(res = r.read_into(b)))
      return Unexpected(res.error());
    ch = static_cast<char>(b);
  } while (std::isspace(ch));
  return {ch};
}

template <>
inline Expected<void> ReadInto<std::string>::read_into(Reader& r,
                                                       std::string& dest) {
  return read_first_non_space_char(r).and_then(
      [&r, &dest](char ch) -> Expected<void> {
        do {
          dest += ch;

          std::byte b;
          auto res = r.read_into(b);
          if (!res && res.error() == Err::UnexpectedEof)
            return {};
          if (!res)
            return Unexpected(res.error());
          ch = static_cast<char>(b);
        } while (!std::isspace(ch));
        return {};
      });
}

template <typename T>
  requires(std::integral<T> || std::floating_point<T>)
class ReadInto<T> {
public:
  static Expected<void> read_into(Reader& r, T& dest) {
    std::string str;
    auto res = r.read_into(str);
    if (!res)
      return res;
    auto from_chars_res =
        std::from_chars(str.c_str(), str.c_str() + str.size(), dest);
    if (from_chars_res.ptr == str.c_str()) {
      return Unexpected(Err::InvalidData);
    }
    return {};
  }
};

template <>
inline Expected<void> WriteFrom<std::byte>::write_from(Writer& w,
                                                       const std::byte& src) {
  return w.write_all(std::span(&src, 1));
}

template <typename T>
  requires std::convertible_to<T, std::string_view>
class WriteFrom<T> {
public:
  static Expected<void> write_from(Writer& w, const T& src) {
    return w.write_all(std::as_bytes(std::span(std::string_view(src))));
  };
};

template <typename T>
  requires(std::integral<T> || std::floating_point<T>)
class WriteFrom<T> {
public:
  static Expected<void> write_from(Writer& w, const T& src) {
    return w.write_from(std::to_string(src));
  };
};

template <>
inline Expected<void> WriteFrom<char>::write_from(Writer& w, const char& src) {
  return WriteFrom<std::byte>::write_from(w, static_cast<std::byte>(src));
}
} // namespace io
