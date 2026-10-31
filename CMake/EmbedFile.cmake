# Usage: cmake -DINPUT=<file> -DOUTPUT=<header> -DSYMBOL=<name> -P EmbedFile.cmake
file(READ "${INPUT}" content HEX)
string(LENGTH "${content}" hex_length)
math(EXPR size "${hex_length} / 2")

# "a1b2..." -> "0xa1,0xb2,..."
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," bytes "${content}")

file(WRITE "${OUTPUT}"
  "// Generated from ${INPUT}. Do not edit.\n"
  "#pragma once\n\n"
  "inline constexpr char ${SYMBOL}[] = {${bytes}0x00};\n"
  "inline constexpr unsigned long long ${SYMBOL}_size = ${size};\n"
)