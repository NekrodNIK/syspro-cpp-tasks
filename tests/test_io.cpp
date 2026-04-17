#include "src/io/io.hpp"
#include "src/io/ioimpl.hpp"
#include <gtest/gtest.h>

using namespace io;

TEST(StringReaderWriter, SimpleWriteRead) {
  StringReaderWriter srw;
  std::string in = "hello";
  auto w = srw.write(std::as_bytes(std::span(in)));
  ASSERT_TRUE(w.has_value());
  EXPECT_EQ(*w, 5);

  std::string out(5, '\0');
  auto r = srw.read(std::as_writable_bytes(std::span(out)));
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ(*r, 5);
  EXPECT_EQ(out, "hello");
}

TEST(StringReaderWriter, ReadEOF) {
  StringReaderWriter srw("hi");
  std::byte buf[10];
  auto r1 = srw.read(buf);
  ASSERT_TRUE(r1.has_value());
  EXPECT_EQ(*r1, 2);

  auto r2 = srw.read(buf);
  ASSERT_FALSE(r2.has_value());
  EXPECT_EQ(r2.error(), Err::UnexpectedEof);
}

TEST(StringBufReaderWriter, BufferedWriteRead) {
  StringBufReaderWriter bufsrw("");
  std::string in = "abcd";
  auto w = bufsrw.write(std::as_bytes(std::span(in)));
  ASSERT_TRUE(w.has_value());
  EXPECT_EQ(*w, 4);
  bufsrw.flush();

  std::string out(4, '\0');
  auto r = bufsrw.read(std::as_writable_bytes(std::span(out)));
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ(*r, 4);
  EXPECT_EQ(out, "abcd");
}

TEST(FileReaderWriter, WriteRead) {
  FILE* f = std::tmpfile();
  ASSERT_NE(f, nullptr);
  FileReaderWriter frw(f);
  std::string s = "test";
  auto w = frw.write(std::as_bytes(std::span(s)));
  ASSERT_TRUE(w.has_value());
  EXPECT_EQ(*w, 4);

  std::rewind(f);
  std::string out(4, '\0');
  auto r = frw.read(std::as_writable_bytes(std::span(out)));
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ(*r, 4);
  EXPECT_EQ(out, "test");

  std::fclose(f);
}

TEST(FileBufReaderWriter, BufferedWriteRead) {
  FILE* f = std::tmpfile();
  ASSERT_NE(f, nullptr);
  FileBufReaderWriter bufrw(f);
  std::string s = "buffered";
  auto w = bufrw.write(std::as_bytes(std::span(s)));
  ASSERT_TRUE(w.has_value());
  EXPECT_EQ(*w, 8);
  bufrw.flush();

  std::rewind(f);
  std::string out(8, '\0');
  auto r = bufrw.read(std::as_writable_bytes(std::span(out)));
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ(*r, 8);
  EXPECT_EQ(out, "buffered");

  std::fclose(f);
}

TEST(StringReaderWriter, StreamOperators) {
  StringReaderWriter srw;

  io::Expected<void> res = srw << 42 << ' ' << "hello" << ' ' << 3.14;
  ASSERT_TRUE(res.has_value());

  int i;
  std::string s;
  double d;
  res = srw >> i >> s >> d;
  ASSERT_TRUE(res.has_value());

  EXPECT_EQ(i, 42);
  EXPECT_EQ(s, "hello");
  EXPECT_DOUBLE_EQ(d, 3.14);
}

TEST(StringReaderWriter, ReadStringStopsAtSpace) {
  StringReaderWriter srw;
  std::string input = "hello world extra";
  srw.write(std::as_bytes(std::span(input)));

  std::string word;
  io::Expected<void> res = srw >> word;
  ASSERT_TRUE(res.has_value());
  EXPECT_EQ(word, "hello");

  std::string rest;
  res = srw >> rest;
  ASSERT_TRUE(res.has_value());
  EXPECT_EQ(rest, "world");
}

TEST(StringBufReaderWriter, StreamOperatorsBuffered) {
  StringBufReaderWriter bufsrw("");

  io::Expected<void> res = bufsrw << "value: " << 100;
  ASSERT_TRUE(res.has_value());
  bufsrw.flush();

  std::string label;
  int num;
  res = bufsrw >> label >> num;
  ASSERT_TRUE(res.has_value());
  EXPECT_EQ(label, "value:");
  EXPECT_EQ(num, 100);
}

TEST(StringReaderWriter, ChainOperatorError) {
  StringReaderWriter srw("123 abc");
  int i;
  std::string s;
  io::Expected<void> res = srw >> i >> s;
  ASSERT_TRUE(res.has_value());
  EXPECT_EQ(i, 123);
  EXPECT_EQ(s, "abc");

  int j;
  res = srw >> j;
  ASSERT_FALSE(res.has_value());
  EXPECT_EQ(res.error(), Err::UnexpectedEof);
}
