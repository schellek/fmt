#pragma once

#include <cstdint>
#include <string_view>

#include "fmt/ostream.hpp"

namespace fmt
{

enum class trait : uint8_t
{
  regular = 0U,
  asChar,
  asShort,
  asLong,
  asLongLong,
  asIntmax_t,
  asSize_t,
  asPtrdiff_t,
  asInvalid
};

struct FormatOptions
{
  static constexpr int NOT_SPECIFIED = -1;

  bool minusFlag;
  bool plusFlag;
  bool spaceFlag;
  bool hashFlag;
  bool zeroFlag;

  int fieldWidth;
  int precision;
};

const char * parseFormatOptions(const char *str, va_list &argList, FormatOptions &formatOptions) noexcept;
const char * parseArgTraitment(const char *str, trait &argTraitment) noexcept;
bool formattingIsRequired(const FormatOptions &formatOptions) noexcept;

} // namespace fmt