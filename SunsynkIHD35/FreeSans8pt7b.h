#ifndef FreeSans8pt7b_H
#define FreeSans8pt7b_H

#ifdef __AVR__
#include <avr/io.h>
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#undef PROGMEM
#define PROGMEM STORE_ATTR
#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)
// PROGMEM is defefind for T4 to place data in specific memory section
#undef PROGMEM
#define PROGMEM
#else
#define PROGMEM
#endif

const uint8_t FreeSans8pt7bBitmaps[] PROGMEM = {
  0x00, 0xFF, 0xD0, 0xB6, 0xD0, 0x13, 0x09, 0x04, 0x8F, 0xF3, 0x21, 0x10,
  0x99, 0xFE, 0x24, 0x12, 0x19, 0x00, 0x10, 0xFB, 0x5C, 0x99, 0x1A, 0x0E,
  0x0B, 0x13, 0x27, 0x5B, 0xE1, 0x00, 0x78, 0x43, 0x31, 0x08, 0x48, 0x21,
  0x60, 0xCD, 0x01, 0xEC, 0x00, 0x27, 0x81, 0xB3, 0x04, 0x84, 0x23, 0x30,
  0x87, 0x80, 0x38, 0x22, 0x11, 0x0D, 0x83, 0x83, 0x83, 0x65, 0x1E, 0x86,
  0x67, 0x9E, 0x60, 0xF0, 0x29, 0x29, 0x24, 0x92, 0x24, 0x88, 0x89, 0x22,
  0x49, 0x24, 0xA4, 0xA0, 0x25, 0x5C, 0xA5, 0x00, 0x10, 0x20, 0x47, 0xF1,
  0x02, 0x04, 0x00, 0xE0, 0xF0, 0x80, 0x11, 0x22, 0x22, 0x44, 0x44, 0x88,
  0x38, 0x8A, 0x0C, 0x18, 0x30, 0x60, 0xC1, 0x82, 0x88, 0xE0, 0x2F, 0x92,
  0x49, 0x24, 0x80, 0x7D, 0x8E, 0x08, 0x10, 0x21, 0x8E, 0x30, 0x41, 0x03,
  0xF8, 0x7D, 0x8E, 0x08, 0x10, 0x63, 0x81, 0x81, 0x83, 0x8D, 0xF0, 0x04,
  0x0C, 0x1C, 0x14, 0x24, 0x64, 0xC4, 0xFF, 0x04, 0x04, 0x04, 0x7E, 0x81,
  0x04, 0x0F, 0xD8, 0xC0, 0x81, 0x83, 0x89, 0xE0, 0x38, 0x8A, 0x0C, 0x0B,
  0xD8, 0xE0, 0xC1, 0x82, 0x88, 0xE0, 0xFE, 0x08, 0x10, 0x41, 0x02, 0x08,
  0x10, 0x20, 0x81, 0x00, 0x7D, 0x8E, 0x0C, 0x1C, 0x6F, 0xB1, 0xC1, 0x83,
  0x8D, 0xF0, 0x38, 0x8A, 0x0C, 0x18, 0x38, 0xDE, 0x81, 0x03, 0x89, 0xE0,
  0x80, 0x80, 0x81, 0xC0, 0x01, 0x07, 0x38, 0xE0, 0xC0, 0x38, 0x0E, 0x03,
  0xFF, 0x00, 0x00, 0xFF, 0x00, 0xE0, 0x38, 0x06, 0x07, 0x1C, 0xE0, 0x80,
  0x7D, 0x8E, 0x08, 0x10, 0x61, 0x86, 0x08, 0x10, 0x00, 0x00, 0x80, 0x07,
  0xC0, 0x61, 0xC2, 0x01, 0x90, 0x02, 0x47, 0x66, 0x23, 0x99, 0x0C, 0x64,
  0x31, 0x90, 0x8E, 0x66, 0x64, 0xEF, 0x18, 0x00, 0x30, 0x00, 0x3F, 0x00,
  0x0C, 0x03, 0x80, 0xA0, 0x6C, 0x13, 0x0C, 0x43, 0x18, 0xFE, 0x60, 0x98,
  0x34, 0x0F, 0x01, 0xFE, 0x41, 0xA0, 0x50, 0x28, 0x14, 0x1B, 0xF9, 0x03,
  0x80, 0xC0, 0x60, 0x7F, 0xE0, 0x1E, 0x30, 0x90, 0x30, 0x08, 0x04, 0x02,
  0x01, 0x00, 0x80, 0xA0, 0x58, 0x43, 0xC0, 0xFE, 0x41, 0xA0, 0x50, 0x18,
  0x0C, 0x06, 0x03, 0x01, 0x80, 0xC0, 0xA0, 0xDF, 0xC0, 0xFF, 0x80, 0x80,
  0x80, 0x80, 0x80, 0xFE, 0x80, 0x80, 0x80, 0x80, 0xFF, 0xFF, 0x02, 0x04,
  0x08, 0x10, 0x3F, 0xC0, 0x81, 0x02, 0x04, 0x00, 0x1F, 0x08, 0x24, 0x07,
  0x00, 0x80, 0x20, 0x08, 0x3E, 0x01, 0x80, 0x50, 0x32, 0x1C, 0x79, 0x80,
  0xC0, 0x60, 0x30, 0x18, 0x0C, 0x07, 0xFF, 0x01, 0x80, 0xC0, 0x60, 0x30,
  0x10, 0xFF, 0xF0, 0x04, 0x10, 0x41, 0x04, 0x10, 0x41, 0x86, 0x1C, 0xDE,
  0x83, 0x43, 0x23, 0x13, 0x09, 0x05, 0x83, 0x61, 0x18, 0x84, 0x43, 0x20,
  0xD0, 0x20, 0x81, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x81, 0x02, 0x07,
  0xF0, 0xC0, 0x78, 0x0F, 0x83, 0xD0, 0x5A, 0x0B, 0x63, 0x64, 0x4C, 0x89,
  0x9B, 0x31, 0x46, 0x38, 0xC7, 0x10, 0xC0, 0xE0, 0x78, 0x36, 0x19, 0x0C,
  0xC6, 0x33, 0x09, 0x86, 0xC1, 0xE0, 0x70, 0x30, 0x1E, 0x18, 0x44, 0x0A,
  0x01, 0x80, 0x60, 0x18, 0x06, 0x01, 0x80, 0x50, 0x26, 0x10, 0x78, 0xFE,
  0x83, 0x81, 0x81, 0x81, 0x83, 0xFE, 0x80, 0x80, 0x80, 0x80, 0x80, 0x1E,
  0x18, 0x44, 0x0A, 0x01, 0x80, 0x60, 0x18, 0x06, 0x01, 0x80, 0x50, 0xA6,
  0x18, 0x7E, 0x00, 0x40, 0xFF, 0x20, 0x68, 0x0A, 0x02, 0x81, 0xBF, 0xC8,
  0x1A, 0x02, 0x80, 0xA0, 0x28, 0x0A, 0x03, 0x3C, 0xC3, 0x81, 0x80, 0xC0,
  0x78, 0x1E, 0x03, 0x81, 0x81, 0x42, 0x3C, 0xFF, 0x84, 0x02, 0x01, 0x00,
  0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x80, 0xC0, 0x60,
  0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x80, 0xC0, 0x50, 0x47, 0xC0, 0xC0,
  0xD0, 0x36, 0x09, 0x86, 0x21, 0x8C, 0x41, 0x30, 0x4C, 0x1A, 0x03, 0x80,
  0xE0, 0x30, 0xC1, 0x06, 0x87, 0x09, 0x0E, 0x13, 0x14, 0x66, 0x68, 0xC4,
  0x99, 0x09, 0x12, 0x12, 0x2C, 0x38, 0x78, 0x30, 0xE0, 0x60, 0xC0, 0xC1,
  0x80, 0x60, 0xD8, 0x63, 0x10, 0x6C, 0x1E, 0x03, 0x00, 0xE0, 0x68, 0x13,
  0x0C, 0x66, 0x19, 0x03, 0x60, 0xD8, 0x23, 0x18, 0x44, 0x1B, 0x02, 0x80,
  0x40, 0x10, 0x04, 0x01, 0x00, 0x40, 0x10, 0x7F, 0x80, 0xC0, 0xC0, 0x60,
  0x60, 0x60, 0x30, 0x30, 0x30, 0x10, 0x18, 0x1F, 0xF0, 0xF2, 0x49, 0x24,
  0x92, 0x49, 0x38, 0x88, 0x44, 0x44, 0x22, 0x22, 0x11, 0xE4, 0x92, 0x49,
  0x24, 0x92, 0x78, 0x30, 0xC5, 0x12, 0x8A, 0x30, 0xFF, 0x80, 0x44, 0x7C,
  0x82, 0x02, 0x02, 0x3E, 0xE2, 0x82, 0x86, 0x7B, 0x81, 0x02, 0x05, 0xCC,
  0x50, 0x60, 0xC1, 0x83, 0x07, 0x15, 0xC0, 0x3C, 0x8E, 0x0C, 0x08, 0x10,
  0x20, 0xA3, 0x3C, 0x01, 0x01, 0x01, 0x3D, 0x43, 0x81, 0x81, 0x81, 0x81,
  0x81, 0x43, 0x3D, 0x3C, 0x42, 0x81, 0x81, 0xFF, 0x80, 0x80, 0x43, 0x3E,
  0x69, 0x74, 0x92, 0x49, 0x20, 0x3A, 0x8E, 0x0C, 0x18, 0x30, 0x60, 0xA3,
  0x3A, 0x06, 0x13, 0xE0, 0x82, 0x08, 0x2E, 0xC6, 0x18, 0x61, 0x86, 0x18,
  0x61, 0x9F, 0xF0, 0x41, 0x55, 0x55, 0x5C, 0x81, 0x02, 0x04, 0x69, 0x96,
  0x3C, 0x68, 0x99, 0x12, 0x34, 0x20, 0xFF, 0xF0, 0xB9, 0xD8, 0xC6, 0x10,
  0xC2, 0x18, 0x43, 0x08, 0x61, 0x0C, 0x21, 0x84, 0x20, 0xBB, 0x18, 0x61,
  0x86, 0x18, 0x61, 0x84, 0x3C, 0x42, 0x81, 0x81, 0x81, 0x81, 0x81, 0x42,
  0x3C, 0xB9, 0x8A, 0x0C, 0x18, 0x30, 0x60, 0xE2, 0xB9, 0x02, 0x04, 0x00,
  0x3D, 0x43, 0x81, 0x81, 0x81, 0x81, 0x81, 0x43, 0x3D, 0x01, 0x01, 0x01,
  0xBC, 0x88, 0x88, 0x88, 0x80, 0x7B, 0x18, 0x20, 0x60, 0x70, 0x61, 0x78,
  0x4B, 0xA4, 0x92, 0x49, 0x80, 0x86, 0x18, 0x61, 0x86, 0x18, 0x63, 0x74,
  0xC3, 0x42, 0x46, 0x66, 0x24, 0x2C, 0x38, 0x18, 0x18, 0xC6, 0x38, 0xC5,
  0x39, 0xA5, 0x26, 0xB4, 0xD2, 0x8E, 0x71, 0x8C, 0x31, 0x80, 0x42, 0xC8,
  0xB0, 0xC1, 0x87, 0x0B, 0x33, 0xC2, 0xC2, 0x42, 0x46, 0x64, 0x24, 0x2C,
  0x38, 0x18, 0x18, 0x10, 0x10, 0x60, 0x7E, 0x0C, 0x30, 0x41, 0x86, 0x08,
  0x20, 0xFE, 0x69, 0x24, 0x94, 0x49, 0x24, 0x98, 0xFF, 0xFE, 0xC9, 0x24,
  0x91, 0x49, 0x24, 0xB0, 0x61, 0x24, 0x38 };

const GFXglyph FreeSans8pt7bGlyphs[] PROGMEM = {
  {     0,   1,   1,   4,    0,    0 },   // 0x20 ' '
  {     1,   1,  12,   5,    2,  -11 },   // 0x21 '!'
  {     3,   3,   4,   5,    1,  -10 },   // 0x22 '"'
  {     5,   9,  11,   9,    0,  -10 },   // 0x23 '#'
  {    18,   7,  13,   9,    1,  -11 },   // 0x24 '$'
  {    30,  14,  11,  14,    0,  -10 },   // 0x25 '%'
  {    50,   9,  11,  11,    1,  -10 },   // 0x26 '&'
  {    63,   1,   4,   3,    1,  -10 },   // 0x27 '''
  {    64,   3,  15,   5,    1,  -11 },   // 0x28 '('
  {    70,   3,  15,   5,    1,  -11 },   // 0x29 ')'
  {    76,   5,   5,   6,    1,  -11 },   // 0x2A '*'
  {    80,   7,   7,   9,    1,   -6 },   // 0x2B '+'
  {    87,   1,   3,   4,    1,    0 },   // 0x2C ','
  {    88,   4,   1,   5,    1,   -4 },   // 0x2D '-'
  {    89,   1,   1,   4,    1,    0 },   // 0x2E '.'
  {    90,   4,  12,   4,    0,  -11 },   // 0x2F '/'
  {    96,   7,  11,   9,    1,  -10 },   // 0x30 '0'
  {   106,   3,  11,   9,    2,  -10 },   // 0x31 '1'
  {   111,   7,  11,   9,    1,  -10 },   // 0x32 '2'
  {   121,   7,  11,   9,    1,  -10 },   // 0x33 '3'
  {   131,   8,  11,   9,    0,  -10 },   // 0x34 '4'
  {   142,   7,  11,   9,    1,  -10 },   // 0x35 '5'
  {   152,   7,  11,   9,    1,  -10 },   // 0x36 '6'
  {   162,   7,  11,   9,    1,  -10 },   // 0x37 '7'
  {   172,   7,  11,   9,    1,  -10 },   // 0x38 '8'
  {   182,   7,  11,   9,    1,  -10 },   // 0x39 '9'
  {   192,   1,   9,   4,    1,   -8 },   // 0x3A ':'
  {   194,   1,  10,   4,    1,   -7 },   // 0x3B ';'
  {   196,   8,   8,   9,    1,   -7 },   // 0x3C '<'
  {   204,   8,   4,   9,    1,   -5 },   // 0x3D '='
  {   208,   8,   8,   9,    1,   -7 },   // 0x3E '>'
  {   216,   7,  12,   9,    1,  -11 },   // 0x3F '?'
  {   227,  14,  14,  16,    1,  -11 },   // 0x40 '@'
  {   252,  10,  12,  11,    0,  -11 },   // 0x41 'A'
  {   267,   9,  12,  11,    1,  -11 },   // 0x42 'B'
  {   281,   9,  12,  11,    1,  -11 },   // 0x43 'C'
  {   295,   9,  12,  11,    1,  -11 },   // 0x44 'D'
  {   309,   8,  12,  10,    1,  -11 },   // 0x45 'E'
  {   321,   7,  12,  10,    1,  -11 },   // 0x46 'F'
  {   332,  10,  12,  12,    1,  -11 },   // 0x47 'G'
  {   347,   9,  12,  12,    1,  -11 },   // 0x48 'H'
  {   361,   1,  12,   4,    2,  -11 },   // 0x49 'I'
  {   363,   6,  12,   8,    1,  -11 },   // 0x4A 'J'
  {   372,   9,  12,  11,    1,  -11 },   // 0x4B 'K'
  {   386,   7,  12,   9,    1,  -11 },   // 0x4C 'L'
  {   397,  11,  12,  14,    1,  -11 },   // 0x4D 'M'
  {   414,   9,  12,  12,    1,  -11 },   // 0x4E 'N'
  {   428,  10,  12,  13,    1,  -11 },   // 0x4F 'O'
  {   443,   8,  12,  11,    1,  -11 },   // 0x50 'P'
  {   455,  10,  13,  13,    1,  -11 },   // 0x51 'Q'
  {   472,  10,  12,  11,    1,  -11 },   // 0x52 'R'
  {   487,   8,  12,  11,    1,  -11 },   // 0x53 'S'
  {   499,   9,  12,  10,    0,  -11 },   // 0x54 'T'
  {   513,   9,  12,  12,    1,  -11 },   // 0x55 'U'
  {   527,  10,  12,  10,    0,  -11 },   // 0x56 'V'
  {   542,  15,  12,  15,    0,  -11 },   // 0x57 'W'
  {   565,  10,  12,  11,    0,  -11 },   // 0x58 'X'
  {   580,  10,  12,  11,    0,  -11 },   // 0x59 'Y'
  {   595,   9,  12,  10,    0,  -11 },   // 0x5A 'Z'
  {   609,   3,  15,   4,    1,  -11 },   // 0x5B '['
  {   615,   4,  12,   4,    0,  -11 },   // 0x5C '\'
  {   621,   3,  15,   4,    0,  -11 },   // 0x5D ']'
  {   627,   6,   6,   7,    1,  -10 },   // 0x5E '^'
  {   632,   9,   1,   9,    0,    3 },   // 0x5F '_'
  {   634,   3,   2,   4,    0,  -11 },   // 0x60 '`'
  {   635,   8,   9,   9,    0,   -8 },   // 0x61 'a'
  {   644,   7,  12,   9,    1,  -11 },   // 0x62 'b'
  {   655,   7,   9,   8,    0,   -8 },   // 0x63 'c'
  {   663,   8,  12,   9,    0,  -11 },   // 0x64 'd'
  {   675,   8,   9,   9,    0,   -8 },   // 0x65 'e'
  {   684,   3,  12,   4,    0,  -11 },   // 0x66 'f'
  {   689,   7,  12,   9,    0,   -8 },   // 0x67 'g'
  {   700,   6,  12,   9,    1,  -11 },   // 0x68 'h'
  {   709,   1,  12,   4,    1,  -11 },   // 0x69 'i'
  {   711,   2,  15,   4,    0,  -11 },   // 0x6A 'j'
  {   715,   7,  12,   8,    1,  -11 },   // 0x6B 'k'
  {   726,   1,  12,   3,    1,  -11 },   // 0x6C 'l'
  {   728,  11,   9,  13,    1,   -8 },   // 0x6D 'm'
  {   741,   6,   9,   9,    1,   -8 },   // 0x6E 'n'
  {   748,   8,   9,   9,    0,   -8 },   // 0x6F 'o'
  {   757,   7,  12,   9,    1,   -8 },   // 0x70 'p'
  {   768,   8,  12,   9,    0,   -8 },   // 0x71 'q'
  {   780,   4,   9,   5,    1,   -8 },   // 0x72 'r'
  {   785,   6,   9,   8,    1,   -8 },   // 0x73 's'
  {   792,   3,  11,   4,    0,  -10 },   // 0x74 't'
  {   797,   6,   9,   9,    1,   -8 },   // 0x75 'u'
  {   804,   8,   9,   8,    0,   -8 },   // 0x76 'v'
  {   813,  11,   9,  12,    0,   -8 },   // 0x77 'w'
  {   826,   7,   9,   8,    0,   -8 },   // 0x78 'x'
  {   834,   8,  12,   8,    0,   -8 },   // 0x79 'y'
  {   846,   7,   9,   8,    0,   -8 },   // 0x7A 'z'
  {   854,   3,  15,   5,    1,  -11 },   // 0x7B '{'
  {   860,   1,  15,   4,    2,  -11 },   // 0x7C '|'
  {   862,   3,  15,   5,    1,  -11 },   // 0x7D '}'
  {   868,   7,   3,   8,    1,   -6 } }; // 0x7E '~'

const GFXfont FreeSans8pt7b PROGMEM = {
  (uint8_t  *)FreeSans8pt7bBitmaps,
  (GFXglyph *)FreeSans8pt7bGlyphs,
  0x20, 0x7E, 19 };

// Approx. 1543 bytes

#endif // FreeSans8pt7b_H
