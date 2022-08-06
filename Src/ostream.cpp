#include <cstring>
#include <cstdlib>
#include <cctype>

#include "fmt/ostream.hpp"
#include "fmt/string_conv.hpp"
#include "format_options.hpp"
#include "formatted_writer.hpp"


using namespace std::string_view_literals;

namespace fmt
{

using ssize_t = std::make_signed_t<size_t>;
using uptrdiff_t = std::make_unsigned_t<ptrdiff_t>;

template <typename T>
static inline T GetVariadicArg(va_list &argList)
{
  static_assert(std::is_arithmetic_v<std::remove_pointer_t<T>> || std::is_same_v<T, void *>);

  using pass_t =  std::conditional_t<std::is_integral_v<T> && (sizeof(T) < sizeof(int)),
                    int,
                    std::conditional_t<std::is_same_v<T, float>,
                      double,
                      T>>;

  return static_cast<T>(va_arg(argList, pass_t));
}

ostream::ostream(write_func *write, flush_func *flush) noexcept
  : m_write{write}, m_flush{flush}
{
  if (write == nullptr)
    exit(EXIT_FAILURE);
}

ostream::size_type ostream::write(char c) noexcept
{
  return (*m_write)(&c, 1);
}

ostream::size_type ostream::write(char c, size_type n) noexcept
{
  constexpr size_type WRITE_CHUNK_SIZE = 20U;
  char toWrite[WRITE_CHUNK_SIZE];
  size_type written = 0U;

  std::fill_n(toWrite, (n < WRITE_CHUNK_SIZE) ? n : WRITE_CHUNK_SIZE, c);

  for (; n > WRITE_CHUNK_SIZE; n -= WRITE_CHUNK_SIZE)
    written += (*m_write)(toWrite, WRITE_CHUNK_SIZE);

  written += (*m_write)(toWrite, n);

  return written;
}

ostream::size_type ostream::write(const char *str, size_type len) noexcept
{
  return (*m_write)(str, len);
}

ostream::size_type ostream::write(std::string_view str) noexcept
{
  return (*m_write)(str.data(), static_cast<size_type>(str.size()));
}

void ostream::flush(void) noexcept
{
  if (m_flush != nullptr)
    (*m_flush)();
}

int ostream::vprintf(const char *str, va_list argList) noexcept
{
  int written = 0;

  FormattedWriter writeFormatted{*this};

  const char *formatBegin;
  FormatOptions formatOptions;
  trait argTraitment;
  std::string_view toBeWritten;
  uint8_t argFlags;
  char c;

  while (*str != '\0')
  {
    if (*str != '%')
    {
      written += write(*(str++));
      continue;
    }

    formatBegin = str++;

    str = parseFormatOptions(str, argList, formatOptions);
    str = parseArgTraitment(str, argTraitment);

    if (argTraitment == trait::asInvalid)
      continue;

    toBeWritten = ""sv;
    argFlags = 0U;

    if (*str == 'u')
    {
      if (argTraitment == trait::regular)
        toBeWritten = toString(GetVariadicArg<unsigned int>(argList));
      else if (argTraitment == trait::asChar)
        toBeWritten = toString(GetVariadicArg<unsigned char>(argList));
      else if (argTraitment == trait::asShort)
        toBeWritten = toString(GetVariadicArg<unsigned short int>(argList));
      else if (argTraitment == trait::asLong)
        toBeWritten = toString(GetVariadicArg<unsigned long int>(argList));
      else if (argTraitment == trait::asLongLong)
        toBeWritten = toString(GetVariadicArg<unsigned long long int>(argList));
      else if (argTraitment == trait::asIntmax_t)
        toBeWritten = toString(GetVariadicArg<uintmax_t>(argList));
      else if (argTraitment == trait::asSize_t)
        toBeWritten = toString(GetVariadicArg<size_t>(argList));
      else if (argTraitment == trait::asPtrdiff_t)
        toBeWritten = toString(GetVariadicArg<uptrdiff_t>(argList));

      argFlags = ArgFlag::Integral | ArgFlag::Decimal;
    }
    else if ((*str == 'd') || (*str == 'i'))
    {
      if (argTraitment == trait::regular)
        toBeWritten = toString(GetVariadicArg<int>(argList));
      else if (argTraitment == trait::asChar)
        toBeWritten = toString(GetVariadicArg<signed char>(argList));
      else if (argTraitment == trait::asShort)
        toBeWritten = toString(GetVariadicArg<short int>(argList));
      else if (argTraitment == trait::asLong)
        toBeWritten = toString(GetVariadicArg<long int>(argList));
      else if (argTraitment == trait::asLongLong)
        toBeWritten = toString(GetVariadicArg<long long int>(argList));
      else if (argTraitment == trait::asIntmax_t)
        toBeWritten = toString(GetVariadicArg<intmax_t>(argList));
      else if (argTraitment == trait::asSize_t)
        toBeWritten = toString(GetVariadicArg<ssize_t>(argList));
      else if (argTraitment == trait::asPtrdiff_t)
        toBeWritten = toString(GetVariadicArg<ptrdiff_t>(argList));

      argFlags = ArgFlag::Signed | ArgFlag::Integral | ArgFlag::Decimal;
    }
    else if ((*str == 'x') || (*str == 'X'))
    {
      const bool upperCase = (*str == 'X');
      const bool prefix = formatOptions.hashFlag;

      if (argTraitment == trait::regular)
        toBeWritten = toHexString(GetVariadicArg<unsigned int>(argList), upperCase, prefix);
      else if (argTraitment == trait::asChar)
        toBeWritten = toHexString(GetVariadicArg<unsigned char>(argList), upperCase, prefix);
      else if (argTraitment == trait::asShort)
        toBeWritten = toHexString(GetVariadicArg<unsigned short int>(argList), upperCase, prefix);
      else if (argTraitment == trait::asLong)
        toBeWritten = toHexString(GetVariadicArg<unsigned long int>(argList), upperCase, prefix);
      else if (argTraitment == trait::asLongLong)
        toBeWritten = toHexString(GetVariadicArg<unsigned long long int>(argList), upperCase, prefix);
      else if (argTraitment == trait::asIntmax_t)
        toBeWritten = toHexString(GetVariadicArg<uintmax_t>(argList), upperCase, prefix);
      else if (argTraitment == trait::asSize_t)
        toBeWritten = toHexString(GetVariadicArg<size_t>(argList), upperCase, prefix);
      else if (argTraitment == trait::asPtrdiff_t)
        toBeWritten = toHexString(GetVariadicArg<uptrdiff_t>(argList), upperCase, prefix);

      argFlags = ArgFlag::Integral | ArgFlag::Hexadecimal;
    }
    else if (*str == 'o')
    {
      const bool prefix = formatOptions.hashFlag;

      if (argTraitment == trait::regular)
        toBeWritten = toOctString(GetVariadicArg<unsigned int>(argList), prefix);
      else if (argTraitment == trait::asChar)
        toBeWritten = toOctString(GetVariadicArg<unsigned char>(argList), prefix);
      else if (argTraitment == trait::asShort)
        toBeWritten = toOctString(GetVariadicArg<unsigned short int>(argList), prefix);
      else if (argTraitment == trait::asLong)
        toBeWritten = toOctString(GetVariadicArg<unsigned long int>(argList), prefix);
      else if (argTraitment == trait::asLongLong)
        toBeWritten = toOctString(GetVariadicArg<unsigned long long int>(argList), prefix);
      else if (argTraitment == trait::asIntmax_t)
        toBeWritten = toOctString(GetVariadicArg<uintmax_t>(argList), prefix);
      else if (argTraitment == trait::asSize_t)
        toBeWritten = toOctString(GetVariadicArg<size_t>(argList), prefix);
      else if (argTraitment == trait::asPtrdiff_t)
        toBeWritten = toOctString(GetVariadicArg<uptrdiff_t>(argList), prefix);

      argFlags = ArgFlag::Integral | ArgFlag::Octal;
    }
    else if (*str == 'p')
    {
      if (void *addr = GetVariadicArg<void *>(argList); addr != nullptr)
      {
        toBeWritten = toHexString(reinterpret_cast<uintptr_t>(addr), false, true);
        argFlags = ArgFlag::Integral | ArgFlag::Hexadecimal;
      }
      else
      {
        toBeWritten = "(nil)"sv;
      }
    }
    else if ((*str == 'f') || (*str == 'F'))
    {
      toBeWritten = toString(GetVariadicArg<float>(argList));

      argFlags = ArgFlag::Signed | ArgFlag::FloatingPoint;
    }
    else if (*str == 's')
    {
      char *s = GetVariadicArg<char *>(argList);
      toBeWritten = (s != nullptr) ? std::string_view{s} : "(null)"sv;
      argFlags = ArgFlag::String;
    }
    else if (*str == 'c')
    {
      c = GetVariadicArg<char>(argList);
      toBeWritten = std::string_view{&c, 1};
    }
    else if (*str == 'n')
    {
      if (argTraitment == trait::regular)
        *GetVariadicArg<int *>(argList) = written;
      else if (argTraitment == trait::asChar)
        *GetVariadicArg<signed char *>(argList) = static_cast<signed char>(written);
      else if (argTraitment == trait::asShort)
        *GetVariadicArg<signed short *>(argList) = static_cast<signed short>(written);
      else if (argTraitment == trait::asLong)
        *GetVariadicArg<long int *>(argList) = static_cast<long int>(written);
      else if (argTraitment == trait::asLongLong)
        *GetVariadicArg<long long int *>(argList) = static_cast<long long int>(written);
      else if (argTraitment == trait::asIntmax_t)
        *GetVariadicArg<intmax_t *>(argList) = static_cast<intmax_t>(written);
      else if (argTraitment == trait::asSize_t)
        *GetVariadicArg<size_t *>(argList) = static_cast<ssize_t>(written);
      else if (argTraitment == trait::asPtrdiff_t)
        *GetVariadicArg<ptrdiff_t *>(argList) = static_cast<ptrdiff_t>(written);
    }
    else if (*str == '%')
    {
      written += write(*str);
    }
    else
    {
      written += write(formatBegin, static_cast<size_type>(str + 1 - formatBegin));
    }

    if (!toBeWritten.empty())
      written += formattingIsRequired(formatOptions) ? writeFormatted(toBeWritten, formatOptions, argFlags) :
                                                       write(toBeWritten);

    ++str;
    continue;
  }

  return written;
}

int ostream::printf(const char *str, ...) noexcept
{
  va_list argList;
  int retval;

  va_start(argList, str);
  retval = vprintf(str, argList);
  va_end(argList);

  return retval;
}

template <typename int_t, std::enable_if_t<std::is_integral_v<int_t>, bool>>
ostream & ostream::operator<<(int_t value) noexcept
{
  write(toString(value));
  return *this;
}

template <typename float_t, std::enable_if_t<std::is_floating_point_v<float_t>, bool>>
ostream & ostream::operator<<(float_t value) noexcept
{
  write(toString(value));
  return *this;
}

ostream & ostream::operator<<(bool value) noexcept
{
  write(value ? "true"sv : "false"sv);
  return *this;
}

ostream & ostream::operator<<(char value) noexcept
{
  write(value);
  return *this;
}

ostream & ostream::operator<<(char *str) noexcept
{
  write(std::string_view{str});
  return *this;
}

ostream & ostream::operator<<(const char *str) noexcept
{
  write(std::string_view{str});
  return *this;
}

ostream & ostream::operator<<(std::string_view str) noexcept
{
  write(str);
  return *this;
}

ostream & ostream::operator<<(const void *addr) noexcept
{
  write(toHexString(reinterpret_cast<uintptr_t>(addr), false, true));
  return *this;
}

ostream & ostream::operator<<(manip_func &function) noexcept
{
  return function(*this);
}

ostream & flush(ostream &stream) noexcept
{
  stream.flush();

  return stream;
}

ostream & endl(ostream &stream) noexcept
{
  stream.write(FMT_ENDL, static_cast<ostream::size_type>(sizeof(FMT_ENDL) - 1U));
  stream.flush();

  return stream;
}

#define INSTANCIATE_TEMPLATE(TYPE) template ostream & ostream::operator<<(TYPE) noexcept
#define INSTANCIATE_INTEGRAL
#define INSTANCIATE_FLOATING_POINT
#include "instanciate_template.hpp"
#undef INSTANCIATE_TEMPLATE

} // namespace fmt
