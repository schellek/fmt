#pragma once

#include <cstdarg>
#include <string_view>

#include "fmt/fmt.h"
#include "fmt/type_traits.hpp"

FMT_BEGIN_NAMESPACE

class ostream
{
public:
  using size_type = fmt_size_type;
  using write_func = size_type(const char *, size_type) noexcept;
  using flush_func = void(void) noexcept;
  using manip_func = ostream &(ostream &) noexcept;

private:
  write_func *const m_write;
  flush_func *const m_flush;

public:
  ostream() = delete;
  ostream(write_func *write, flush_func *flush = nullptr) noexcept;

  size_type write(char c) noexcept;
  size_type write(char c, size_type n) noexcept;
  size_type write(const char *str, size_type len) noexcept;
  size_type write(std::string_view str) noexcept;

  void flush(void) noexcept;

  int vprintf(const char *str, va_list args) noexcept;
  int printf(FMT_PRINTF_FMTSTR const char *str, ...) noexcept FMT_PRINTF_FUNC(2);

  ostream & operator<<(bool value) noexcept;
  ostream & operator<<(char value) noexcept;
  ostream & operator<<(char *str) noexcept;
  ostream & operator<<(const char *str) noexcept;
  ostream & operator<<(std::string_view str) noexcept;
  ostream & operator<<(std::nullptr_t) noexcept;

  template <typename T, EnableIfT<IsIntegralV<T>> = true>
  ostream & operator<<(T value) noexcept;

  template <typename T, EnableIfT<IsFloatingPointV<T>> = true>
  ostream & operator<<(T value) noexcept;

  template <typename T, EnableIfT<IsPointerV<T>> = true>
  ostream & operator<<(T value) noexcept;

  template <typename T, EnableIfT<IsSmartPointerV<T>> = true>
  ostream & operator<<(T &value) noexcept;

  ostream & operator<<(const void *p) noexcept;
  ostream & operator<<(manip_func &function) noexcept;
};

ostream & flush(ostream &stream) noexcept;
ostream & endl(ostream &stream) noexcept;

extern ostream cout;

FMT_END_NAMESPACE

#include "ostream.inl"
