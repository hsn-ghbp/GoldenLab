/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --size 16 --font C:/Users/kimiya/SquareLine/assets/Vazir.ttf -o C:/Users/kimiya/SquareLine/assets\ui_font_Font1.c --format lvgl -r 0x20-0x7f -r 0x600-0x6ff --symbols سلام --no-compress --no-prefilter
 ******************************************************************************/

#include "../ui.h"

#ifndef UI_FONT_FONT1
#define UI_FONT_FONT1 1
#endif

#if UI_FONT_FONT1

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0x55, 0x55, 0xf,

    /* U+0022 "\"" */
    0xb6, 0xd0,

    /* U+0023 "#" */
    0xb, 0x9, 0x4, 0x8f, 0xf1, 0x20, 0x90, 0x48,
    0x48, 0xff, 0x12, 0x9, 0x4, 0x80,

    /* U+0024 "$" */
    0x10, 0x20, 0xf2, 0x34, 0x28, 0x18, 0x1c, 0x4,
    0x6, 0xe, 0x37, 0xc2, 0x4, 0x0,

    /* U+0025 "%" */
    0x70, 0x22, 0x48, 0x92, 0x28, 0x72, 0x1, 0x0,
    0x98, 0x29, 0x12, 0x4c, 0x92, 0x24, 0x6,

    /* U+0026 "&" */
    0x38, 0x22, 0x11, 0x8, 0x86, 0x81, 0xc1, 0xc1,
    0xb2, 0x8d, 0x43, 0xb1, 0xcf, 0xa0,

    /* U+0027 "'" */
    0xf0,

    /* U+0028 "(" */
    0x32, 0x44, 0x88, 0x88, 0x88, 0x88, 0x44, 0x62,
    0x10,

    /* U+0029 ")" */
    0xc4, 0x22, 0x11, 0x11, 0x11, 0x11, 0x22, 0x64,
    0x0,

    /* U+002A "*" */
    0x10, 0x23, 0x51, 0xe3, 0x85, 0x0, 0x0,

    /* U+002B "+" */
    0x10, 0x10, 0x10, 0xff, 0x10, 0x10, 0x10, 0x10,

    /* U+002C "," */
    0x56,

    /* U+002D "-" */
    0xf0,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x4, 0x30, 0x82, 0x10, 0x41, 0x8, 0x21, 0x84,
    0x10, 0xc0,

    /* U+0030 "0" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x83,
    0x5, 0x11, 0xc0,

    /* U+0031 "1" */
    0x1f, 0x91, 0x11, 0x11, 0x11, 0x11,

    /* U+0032 "2" */
    0x3c, 0x8f, 0xc, 0x10, 0x20, 0x83, 0xc, 0x30,
    0xc1, 0x7, 0xf0,

    /* U+0033 "3" */
    0x3c, 0xcd, 0x8, 0x10, 0x63, 0x81, 0x81, 0x2,
    0x85, 0x91, 0xe0,

    /* U+0034 "4" */
    0x4, 0xc, 0x1c, 0x14, 0x34, 0x24, 0x44, 0xc4,
    0xff, 0x4, 0x4, 0x4,

    /* U+0035 "5" */
    0x7e, 0x81, 0x2, 0x7, 0xc8, 0x80, 0x81, 0x3,
    0x85, 0x11, 0xe0,

    /* U+0036 "6" */
    0x1c, 0x41, 0x4, 0xf, 0x98, 0xa0, 0xc1, 0x83,
    0x5, 0x11, 0xc0,

    /* U+0037 "7" */
    0xff, 0x3, 0x2, 0x6, 0x4, 0xc, 0x8, 0x18,
    0x18, 0x10, 0x30, 0x20,

    /* U+0038 "8" */
    0x7d, 0x8e, 0xc, 0x1c, 0x6f, 0xb1, 0xc1, 0x83,
    0x5, 0x11, 0xc0,

    /* U+0039 "9" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x51, 0x9d, 0x2,
    0x8, 0x33, 0x80,

    /* U+003A ":" */
    0xf0, 0x3, 0xc0,

    /* U+003B ";" */
    0x6c, 0x0, 0x2, 0x4b, 0x0,

    /* U+003C "<" */
    0x6, 0x3d, 0xe6, 0xe, 0x7, 0x81, 0x80,

    /* U+003D "=" */
    0xfe, 0x0, 0x0, 0xf, 0xe0,

    /* U+003E ">" */
    0x81, 0xc0, 0xe0, 0x31, 0xce, 0x30, 0x0,

    /* U+003F "?" */
    0x7b, 0x38, 0x41, 0x4, 0x21, 0x8, 0x20, 0x0,
    0x8,

    /* U+0040 "@" */
    0xf, 0x80, 0x83, 0x8, 0x4, 0x87, 0x24, 0x48,
    0xc4, 0x46, 0x22, 0x31, 0x11, 0x88, 0x8c, 0x4c,
    0x62, 0x64, 0x9d, 0xc4, 0x0, 0x18, 0x0, 0x3e,
    0x0,

    /* U+0041 "A" */
    0xc, 0x3, 0x0, 0xe0, 0x68, 0x13, 0x4, 0xc3,
    0x10, 0x86, 0x7f, 0x98, 0x24, 0xf, 0x3,

    /* U+0042 "B" */
    0xfd, 0xe, 0xc, 0x18, 0x7f, 0xa1, 0xc1, 0x83,
    0x6, 0x1f, 0xe0,

    /* U+0043 "C" */
    0x3c, 0x42, 0x43, 0x81, 0x80, 0x80, 0x80, 0x80,
    0x81, 0x43, 0x42, 0x3c,

    /* U+0044 "D" */
    0xf8, 0x86, 0x82, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x82, 0x86, 0xf8,

    /* U+0045 "E" */
    0xff, 0x2, 0x4, 0x8, 0x1f, 0xa0, 0x40, 0x81,
    0x2, 0x7, 0xf0,

    /* U+0046 "F" */
    0xff, 0x2, 0x4, 0x8, 0x10, 0x3f, 0x40, 0x81,
    0x2, 0x4, 0x0,

    /* U+0047 "G" */
    0x3e, 0x30, 0x90, 0x30, 0x8, 0x4, 0x2, 0x1f,
    0x1, 0x80, 0xa0, 0x58, 0x63, 0xe0,

    /* U+0048 "H" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0xff, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81,

    /* U+0049 "I" */
    0xff, 0xf0,

    /* U+004A "J" */
    0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x81, 0x2,
    0x85, 0x99, 0xe0,

    /* U+004B "K" */
    0x82, 0x86, 0x8c, 0x98, 0xb0, 0xf0, 0xf0, 0x98,
    0x8c, 0x84, 0x86, 0x83,

    /* U+004C "L" */
    0x82, 0x8, 0x20, 0x82, 0x8, 0x20, 0x82, 0x8,
    0x3f,

    /* U+004D "M" */
    0xc0, 0x78, 0xf, 0x83, 0xf0, 0x7a, 0xb, 0x63,
    0x64, 0x4c, 0xd9, 0x9b, 0x31, 0x46, 0x38, 0xc2,
    0x10,

    /* U+004E "N" */
    0x81, 0xc1, 0xe1, 0xe1, 0xb1, 0x99, 0x99, 0x8d,
    0x85, 0x87, 0x83, 0x81,

    /* U+004F "O" */
    0x3e, 0x31, 0x90, 0x50, 0x18, 0xc, 0x6, 0x3,
    0x1, 0x80, 0xa0, 0x98, 0xc7, 0xc0,

    /* U+0050 "P" */
    0xfc, 0x82, 0x81, 0x81, 0x81, 0x82, 0xfc, 0x80,
    0x80, 0x80, 0x80, 0x80,

    /* U+0051 "Q" */
    0x3e, 0x31, 0x90, 0x50, 0x18, 0xc, 0x6, 0x3,
    0x1, 0x80, 0xa0, 0xd8, 0xc3, 0xe0, 0x18, 0x4,

    /* U+0052 "R" */
    0xfc, 0x41, 0x20, 0x50, 0x28, 0x14, 0x1b, 0xf9,
    0xc, 0x86, 0x41, 0x20, 0xd0, 0x20,

    /* U+0053 "S" */
    0x1c, 0x62, 0x41, 0x40, 0x60, 0x3c, 0xe, 0x3,
    0x1, 0x81, 0x43, 0x3c,

    /* U+0054 "T" */
    0xff, 0x84, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0x8, 0x4, 0x2, 0x1, 0x0,

    /* U+0055 "U" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x42, 0x3c,

    /* U+0056 "V" */
    0xc0, 0xd0, 0x26, 0x9, 0x86, 0x21, 0xc, 0x41,
    0x30, 0x48, 0x1a, 0x3, 0x80, 0xc0, 0x30,

    /* U+0057 "W" */
    0x43, 0xd, 0xc, 0x36, 0x30, 0x98, 0xe2, 0x26,
    0x98, 0x92, 0x62, 0x4d, 0xd, 0x14, 0x3c, 0x50,
    0x61, 0xc1, 0x86, 0x6, 0x8,

    /* U+0058 "X" */
    0x61, 0x98, 0x63, 0x30, 0x48, 0x1e, 0x3, 0x0,
    0xc0, 0x78, 0x12, 0xc, 0xc6, 0x19, 0x82,

    /* U+0059 "Y" */
    0xc1, 0xa0, 0x98, 0xc4, 0x43, 0x60, 0xa0, 0x70,
    0x10, 0x8, 0x4, 0x2, 0x1, 0x0,

    /* U+005A "Z" */
    0xff, 0x3, 0x6, 0x4, 0xc, 0x18, 0x18, 0x30,
    0x20, 0x60, 0xc0, 0xff,

    /* U+005B "[" */
    0xf2, 0x49, 0x24, 0x92, 0x49, 0x27,

    /* U+005C "\\" */
    0xc1, 0x6, 0x8, 0x20, 0xc1, 0x4, 0x18, 0x20,
    0xc1, 0x4,

    /* U+005D "]" */
    0xe4, 0x92, 0x49, 0x24, 0x92, 0x4f,

    /* U+005E "^" */
    0x10, 0xc3, 0x9a, 0x4d, 0x10,

    /* U+005F "_" */
    0xfe,

    /* U+0060 "`" */
    0xc6, 0x20,

    /* U+0061 "a" */
    0x7b, 0x38, 0x41, 0x7f, 0x18, 0x63, 0x7c,

    /* U+0062 "b" */
    0x81, 0x2, 0x5, 0xcc, 0x50, 0x60, 0xc1, 0x83,
    0x7, 0x17, 0xc0,

    /* U+0063 "c" */
    0x3c, 0x8e, 0x1c, 0x8, 0x10, 0x21, 0xa3, 0x3c,

    /* U+0064 "d" */
    0x2, 0x4, 0x9, 0xf4, 0x70, 0x60, 0xc1, 0x83,
    0x5, 0x19, 0xf0,

    /* U+0065 "e" */
    0x38, 0x8a, 0x1c, 0x3f, 0xf0, 0x20, 0x23, 0x3c,

    /* U+0066 "f" */
    0x19, 0x8, 0x4f, 0x90, 0x84, 0x21, 0x8, 0x42,
    0x0,

    /* U+0067 "g" */
    0x3e, 0x8e, 0xc, 0x18, 0x30, 0x60, 0xa3, 0x3e,
    0x7, 0x13, 0xc0,

    /* U+0068 "h" */
    0x82, 0x8, 0x2e, 0xc6, 0x18, 0x61, 0x86, 0x18,
    0x61,

    /* U+0069 "i" */
    0x9f, 0xf0,

    /* U+006A "j" */
    0x20, 0x12, 0x49, 0x24, 0x92, 0x70,

    /* U+006B "k" */
    0x81, 0x2, 0x4, 0x69, 0x96, 0x38, 0x70, 0xb1,
    0x32, 0x34, 0x60,

    /* U+006C "l" */
    0xff, 0xf0,

    /* U+006D "m" */
    0xf9, 0xd9, 0xc6, 0x10, 0xc2, 0x18, 0x43, 0x8,
    0x61, 0xc, 0x21, 0x84, 0x20,

    /* U+006E "n" */
    0xbb, 0x18, 0x61, 0x86, 0x18, 0x61, 0x84,

    /* U+006F "o" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xa2, 0x38,

    /* U+0070 "p" */
    0xf9, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xe2, 0xf9,
    0x2, 0x4, 0x0,

    /* U+0071 "q" */
    0x3e, 0x8e, 0xc, 0x18, 0x30, 0x60, 0xa3, 0x3e,
    0x4, 0x8, 0x10,

    /* U+0072 "r" */
    0xfc, 0x88, 0x88, 0x88, 0x80,

    /* U+0073 "s" */
    0x38, 0x8b, 0x13, 0x3, 0xc1, 0xb1, 0xe2, 0x78,

    /* U+0074 "t" */
    0x21, 0x3e, 0x42, 0x10, 0x84, 0x21, 0x6,

    /* U+0075 "u" */
    0x86, 0x18, 0x61, 0x86, 0x18, 0x63, 0x7c,

    /* U+0076 "v" */
    0xc2, 0x85, 0x9b, 0x22, 0x45, 0x8e, 0xc, 0x18,

    /* U+0077 "w" */
    0xc6, 0x34, 0x62, 0x46, 0x26, 0xf6, 0x29, 0x42,
    0x94, 0x39, 0xc3, 0xc, 0x10, 0x80,

    /* U+0078 "x" */
    0x42, 0x66, 0x3c, 0x18, 0x18, 0x18, 0x2c, 0x66,
    0x42,

    /* U+0079 "y" */
    0x43, 0x62, 0x66, 0x26, 0x24, 0x3c, 0x18, 0x18,
    0x18, 0x10, 0x30, 0x60,

    /* U+007A "z" */
    0xfe, 0xc, 0x30, 0xc3, 0x6, 0x18, 0x60, 0xfe,

    /* U+007B "{" */
    0x19, 0x8, 0x42, 0x10, 0x98, 0x21, 0x8, 0x42,
    0x10, 0x60,

    /* U+007C "|" */
    0xff, 0xfc,

    /* U+007D "}" */
    0xc1, 0x8, 0x42, 0x10, 0xc3, 0x31, 0x8, 0x42,
    0x13, 0x0,

    /* U+007E "~" */
    0x70, 0xcc, 0xe1, 0xc0,

    /* U+060C "،" */
    0x29, 0x36,

    /* U+0615 "ؕ" */
    0x42, 0x16, 0xdf, 0x0,

    /* U+061B "؛" */
    0x29, 0x36, 0x3, 0x60,

    /* U+061F "؟" */
    0x7b, 0x18, 0x60, 0xc1, 0x81, 0x4, 0x0, 0x61,
    0x80,

    /* U+0621 "ء" */
    0x74, 0x21, 0xa7, 0xe0,

    /* U+0622 "آ" */
    0xfd, 0x80, 0x42, 0x10, 0x84, 0x21, 0x8, 0x42,
    0x0,

    /* U+0623 "أ" */
    0x72, 0x70, 0x92, 0x49, 0x24, 0x80,

    /* U+0624 "ؤ" */
    0x32, 0x10, 0x64, 0x1, 0xd2, 0x8c, 0x5e, 0x11,
    0x7a, 0x0,

    /* U+0625 "إ" */
    0x49, 0x24, 0x92, 0x49, 0xf, 0x3e,

    /* U+0626 "ئ" */
    0x18, 0x8, 0x2, 0x0, 0xe0, 0x0, 0xd0, 0x4c,
    0x22, 0x8, 0x83, 0xe0, 0x18, 0x5, 0x6, 0x3f,
    0x0,

    /* U+0627 "ا" */
    0xff, 0xe0,

    /* U+0628 "ب" */
    0x80, 0xc, 0x0, 0x60, 0x2, 0xc0, 0x33, 0xfe,
    0x0, 0x0, 0x8, 0x0, 0xe0, 0x2, 0x0,

    /* U+0629 "ة" */
    0xb, 0xe4, 0x80, 0x21, 0xc7, 0xb3, 0x86, 0x17,
    0x80,

    /* U+062A "ت" */
    0x5, 0x0, 0x6c, 0x0, 0x1, 0x0, 0x18, 0x0,
    0xc0, 0x5, 0x80, 0x67, 0xfc,

    /* U+062B "ث" */
    0x2, 0x0, 0x38, 0x0, 0x80, 0x1b, 0x0, 0x0,
    0x40, 0x6, 0x0, 0x30, 0x1, 0x60, 0x19, 0xff,
    0x0,

    /* U+062C "ج" */
    0x70, 0x47, 0x1, 0xc7, 0x6, 0x2, 0x2, 0x1,
    0x18, 0x80, 0x40, 0x10, 0x7, 0xf0,

    /* U+062D "ح" */
    0x70, 0x47, 0x1, 0xc7, 0x6, 0x2, 0x2, 0x1,
    0x0, 0x80, 0x40, 0x10, 0x7, 0xf0,

    /* U+062E "خ" */
    0x0, 0x18, 0x0, 0x0, 0x7, 0x4, 0x70, 0x1c,
    0x70, 0x60, 0x20, 0x20, 0x10, 0x8, 0x4, 0x1,
    0x0, 0x7f,

    /* U+062F "د" */
    0x30, 0xe0, 0x81, 0x4, 0x3f, 0x80,

    /* U+0630 "ذ" */
    0x21, 0x80, 0x0, 0x30, 0xe0, 0x81, 0x4, 0x3f,
    0x80,

    /* U+0631 "ر" */
    0x18, 0x42, 0x10, 0x8b, 0x90,

    /* U+0632 "ز" */
    0x10, 0x80, 0x1, 0x84, 0x21, 0x8, 0xb9, 0x0,

    /* U+0633 "س" */
    0x0, 0x0, 0x40, 0x11, 0x1c, 0x4, 0x46, 0x1,
    0x11, 0x80, 0x7b, 0xa0, 0x10, 0x8, 0x4, 0x1,
    0x6, 0x0, 0x3f, 0x0, 0x0,

    /* U+0634 "ش" */
    0x0, 0x4, 0x0, 0x1, 0x0, 0x1, 0x20, 0x0,
    0x78, 0x0, 0x0, 0x0, 0x0, 0x10, 0x4, 0x47,
    0x1, 0x11, 0x80, 0x44, 0x60, 0x1e, 0xe8, 0x4,
    0x2, 0x1, 0x0, 0x41, 0x80, 0xf, 0xc0, 0x0,

    /* U+0635 "ص" */
    0x0, 0x3, 0xc0, 0x0, 0xc6, 0x3, 0x30, 0xc0,
    0x2c, 0x18, 0x5, 0x5, 0x0, 0xff, 0x20, 0x10,
    0x4, 0x6, 0x0, 0x41, 0x80, 0x7, 0xe0, 0x0,

    /* U+0636 "ض" */
    0x0, 0x0, 0x80, 0x0, 0x30, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x3c, 0x0, 0xc, 0x60, 0x33,
    0xc, 0x2, 0xc1, 0x80, 0x50, 0x50, 0xf, 0xf2,
    0x1, 0x0, 0x40, 0x60, 0x4, 0x18, 0x0, 0x7e,
    0x0, 0x0,

    /* U+0637 "ط" */
    0x20, 0x8, 0x2, 0x0, 0x80, 0x20, 0x9, 0xe3,
    0xcc, 0xe1, 0x30, 0x48, 0x2f, 0xf0,

    /* U+0638 "ظ" */
    0x20, 0x8, 0xc2, 0x30, 0x80, 0x20, 0x9, 0xe3,
    0xcc, 0xe1, 0x30, 0x48, 0x2f, 0xf0,

    /* U+0639 "ع" */
    0x1e, 0x11, 0x8, 0x4, 0x1, 0xf3, 0x3, 0x1,
    0x0, 0x80, 0x40, 0x20, 0xc, 0x3, 0xf0,

    /* U+063A "غ" */
    0x0, 0xc, 0x2, 0x0, 0x1, 0xe1, 0x10, 0x80,
    0x40, 0x1f, 0x30, 0x30, 0x10, 0x8, 0x4, 0x2,
    0x0, 0xc0, 0x3f, 0x0,

    /* U+0640 "ـ" */
    0xe0,

    /* U+0641 "ف" */
    0x0, 0x0, 0x3, 0x0, 0x8, 0x0, 0x0, 0x7,
    0x0, 0x4a, 0x2, 0x30, 0x11, 0x80, 0x7c, 0x0,
    0x58, 0x6, 0x7f, 0xe0,

    /* U+0642 "ق" */
    0x0, 0x1, 0xe0, 0x8, 0x0, 0x3, 0xa1, 0x38,
    0x4e, 0x11, 0x83, 0xe0, 0x18, 0xd, 0x6, 0x3f,
    0x0,

    /* U+0643 "ك" */
    0x0, 0x8, 0x38, 0x41, 0x2, 0x6, 0x10, 0x18,
    0xc3, 0x86, 0x0, 0x30, 0x1, 0x80, 0x1b, 0x1,
    0x87, 0xf0,

    /* U+0644 "ل" */
    0x0, 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2,
    0x1, 0xc0, 0xc0, 0x60, 0x30, 0x18, 0xa, 0x8,
    0xf8,

    /* U+0645 "م" */
    0xe, 0x11, 0x31, 0x79, 0xce, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80,

    /* U+0646 "ن" */
    0x0, 0x3, 0x0, 0xf, 0x1, 0x80, 0x60, 0x18,
    0x6, 0x3, 0x41, 0x8f, 0xc0,

    /* U+0647 "ه" */
    0x21, 0xc7, 0xb3, 0x86, 0x17, 0x80,

    /* U+0648 "و" */
    0x74, 0xa3, 0x17, 0x84, 0x5e, 0x80,

    /* U+0649 "ى" */
    0x0, 0xd0, 0x4c, 0x22, 0x8, 0x83, 0xe0, 0x18,
    0x5, 0x6, 0x3f, 0x0,

    /* U+064A "ي" */
    0x0, 0xd0, 0x4c, 0x22, 0x8, 0x83, 0xe0, 0x18,
    0x5, 0x6, 0x3f, 0x0, 0x1, 0x20, 0x68,

    /* U+064B "ً" */
    0x3c, 0x78,

    /* U+064C "ٌ" */
    0x32, 0xaf, 0x4c, 0x0,

    /* U+064D "ٍ" */
    0x79, 0xe0,

    /* U+064E "َ" */
    0x1e,

    /* U+064F "ُ" */
    0x66, 0x3c,

    /* U+0650 "ِ" */
    0x16, 0x80,

    /* U+0651 "ّ" */
    0x3b, 0xd0,

    /* U+0652 "ْ" */
    0xf7, 0x80,

    /* U+0653 "ٓ" */
    0xfc, 0x0,

    /* U+0654 "ٔ" */
    0x73, 0xe0,

    /* U+0655 "ٕ" */
    0x73, 0xe0,

    /* U+0657 "ٗ" */
    0x19, 0x38, 0xc6, 0x0,

    /* U+065A "ٚ" */
    0x8a, 0x88,

    /* U+0660 "٠" */
    0x74, 0xe7, 0x37, 0x0,

    /* U+0661 "١" */
    0xa9, 0x55, 0x54,

    /* U+0662 "٢" */
    0x83, 0x7, 0xa, 0x27, 0xc8, 0x10, 0x20, 0x40,
    0x81, 0x0,

    /* U+0663 "٣" */
    0x81, 0x89, 0x89, 0xc9, 0x76, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40,

    /* U+0664 "٤" */
    0x39, 0xe4, 0x10, 0x78, 0xe7, 0x30, 0x83, 0x7,
    0xc0,

    /* U+0665 "٥" */
    0x30, 0x38, 0x6c, 0x46, 0xc3, 0x81, 0x81, 0x81,
    0x42, 0x3c,

    /* U+0666 "٦" */
    0x82, 0xfe, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
    0x3, 0x1, 0x1,

    /* U+0667 "٧" */
    0x81, 0xc3, 0x43, 0x62, 0x66, 0x24, 0x34, 0x3c,
    0x18, 0x18, 0x18,

    /* U+0668 "٨" */
    0x18, 0x18, 0x18, 0x3c, 0x34, 0x24, 0x66, 0x62,
    0x43, 0xc3, 0x81,

    /* U+0669 "٩" */
    0x7b, 0x38, 0xe1, 0x85, 0xf0, 0x41, 0x4, 0x10,
    0x40,

    /* U+066A "٪" */
    0x5, 0xcb, 0xb0, 0x41, 0x82, 0xc, 0x10, 0x6e,
    0x9f, 0x0,

    /* U+066B "٫" */
    0x13, 0x26, 0x44, 0x88,

    /* U+066C "٬" */
    0xdd, 0x28,

    /* U+066D "٭" */
    0x0, 0x4, 0x6, 0xf, 0xe3, 0xe1, 0xe0, 0xd0,
    0x84,

    /* U+066E "ٮ" */
    0x80, 0xc, 0x0, 0x60, 0x2, 0xc0, 0x33, 0xfe,
    0x0,

    /* U+066F "ٯ" */
    0x3, 0xa1, 0x38, 0x4e, 0x11, 0x83, 0xe0, 0x18,
    0xd, 0x6, 0x3f, 0x0,

    /* U+0670 "ٰ" */
    0xe0,

    /* U+0674 "ٴ" */
    0x73, 0xe0,

    /* U+067E "پ" */
    0x80, 0xc, 0x0, 0x60, 0x2, 0xc0, 0x33, 0xfe,
    0x0, 0x0, 0x0, 0x1, 0xb0, 0x2, 0x0, 0x38,
    0x0, 0x80,

    /* U+0686 "چ" */
    0x70, 0x47, 0x1, 0xc7, 0x6, 0x2, 0x12, 0x7d,
    0x4, 0x8c, 0x40, 0x10, 0x7, 0xf0,

    /* U+0698 "ژ" */
    0x10, 0x42, 0x9a, 0x0, 0x1, 0x82, 0x8, 0x20,
    0x84, 0xe2, 0x0,

    /* U+06A1 "ڡ" */
    0x0, 0x70, 0x4, 0xa0, 0x23, 0x1, 0x18, 0x7,
    0xc0, 0x5, 0x80, 0x67, 0xfe,

    /* U+06A9 "ک" */
    0x0, 0x4, 0x0, 0xf0, 0xe, 0x0, 0x40, 0x1,
    0x80, 0x3, 0x8, 0x2, 0x20, 0x4, 0x80, 0x11,
    0x80, 0xc3, 0xfc, 0x0,

    /* U+06AF "گ" */
    0x0, 0x8, 0x3, 0x80, 0x62, 0x0, 0x70, 0x1e,
    0x0, 0x80, 0x6, 0x0, 0x18, 0x80, 0x64, 0x0,
    0xa0, 0x4, 0xc0, 0x63, 0xfc, 0x0,

    /* U+06BA "ں" */
    0x0, 0xf0, 0x18, 0x6, 0x1, 0x80, 0x60, 0x34,
    0x18, 0xfc,

    /* U+06C0 "ۀ" */
    0x19, 0x87, 0x86, 0xf0, 0x0, 0x1c, 0x7b, 0xec,
    0xe1, 0x85, 0xe0,

    /* U+06CA "ۊ" */
    0x6, 0xc0, 0x7, 0x4a, 0x31, 0x78, 0x45, 0xe8,
    0x0,

    /* U+06CC "ی" */
    0x0, 0xd0, 0x4c, 0x22, 0x8, 0x83, 0xe0, 0x18,
    0x5, 0x6, 0x3f, 0x0,

    /* U+06D5 "ە" */
    0x21, 0xc7, 0xb3, 0x86, 0x17, 0x80,

    /* U+06F0 "۰" */
    0x74, 0xe7, 0x37, 0x0,

    /* U+06F1 "۱" */
    0xa9, 0x55, 0x54,

    /* U+06F2 "۲" */
    0x83, 0x7, 0xa, 0x27, 0xc8, 0x10, 0x20, 0x40,
    0x81, 0x0,

    /* U+06F3 "۳" */
    0x81, 0x89, 0x89, 0xc9, 0x76, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40,

    /* U+06F4 "۴" */
    0x9d, 0x62, 0x83, 0x6, 0xf, 0xd0, 0x20, 0x40,
    0x81, 0x0,

    /* U+06F5 "۵" */
    0x10, 0x18, 0x3c, 0x24, 0x46, 0x42, 0xc3, 0x99,
    0x99, 0x99, 0x66,

    /* U+06F6 "۶" */
    0x79, 0x82, 0x4, 0x1f, 0xe7, 0xc, 0x30, 0x41,
    0x82, 0x0,

    /* U+06F7 "۷" */
    0x81, 0xc3, 0x43, 0x62, 0x66, 0x24, 0x34, 0x3c,
    0x18, 0x18, 0x18,

    /* U+06F8 "۸" */
    0x18, 0x18, 0x18, 0x3c, 0x34, 0x24, 0x66, 0x62,
    0x43, 0xc3, 0x81,

    /* U+06F9 "۹" */
    0x7b, 0x38, 0xe1, 0x85, 0xf0, 0x41, 0x4, 0x10,
    0x40
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 70, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 80, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 82, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 6, .adv_w = 159, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 20, .adv_w = 144, .box_w = 7, .box_h = 15, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 34, .adv_w = 188, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 49, .adv_w = 159, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 63, .adv_w = 45, .box_w = 1, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 64, .adv_w = 88, .box_w = 4, .box_h = 17, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 73, .adv_w = 89, .box_w = 4, .box_h = 17, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 82, .adv_w = 110, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 89, .adv_w = 145, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 97, .adv_w = 50, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 98, .adv_w = 71, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 99, .adv_w = 80, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 100, .adv_w = 106, .box_w = 6, .box_h = 13, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 110, .adv_w = 144, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 121, .adv_w = 144, .box_w = 4, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 127, .adv_w = 144, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 138, .adv_w = 144, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 149, .adv_w = 144, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 161, .adv_w = 144, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 172, .adv_w = 144, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 183, .adv_w = 144, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 195, .adv_w = 144, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 206, .adv_w = 144, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 217, .adv_w = 80, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 220, .adv_w = 54, .box_w = 3, .box_h = 12, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 225, .adv_w = 130, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 232, .adv_w = 141, .box_w = 7, .box_h = 5, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 237, .adv_w = 134, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 244, .adv_w = 121, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 253, .adv_w = 230, .box_w = 13, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 278, .adv_w = 167, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 293, .adv_w = 159, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 304, .adv_w = 167, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 316, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 328, .adv_w = 146, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 339, .adv_w = 142, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 350, .adv_w = 174, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 364, .adv_w = 183, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 376, .adv_w = 70, .box_w = 1, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 378, .adv_w = 141, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 389, .adv_w = 161, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 401, .adv_w = 138, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 410, .adv_w = 224, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 427, .adv_w = 183, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 439, .adv_w = 176, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 453, .adv_w = 162, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 465, .adv_w = 176, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 481, .adv_w = 158, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 495, .adv_w = 152, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 507, .adv_w = 153, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 521, .adv_w = 166, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 533, .adv_w = 163, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 548, .adv_w = 227, .box_w = 14, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 569, .adv_w = 161, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 584, .adv_w = 154, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 598, .adv_w = 153, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 610, .adv_w = 68, .box_w = 3, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 616, .adv_w = 105, .box_w = 6, .box_h = 13, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 626, .adv_w = 68, .box_w = 3, .box_h = 16, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 632, .adv_w = 107, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 637, .adv_w = 116, .box_w = 7, .box_h = 1, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 638, .adv_w = 79, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 10},
    {.bitmap_index = 640, .adv_w = 139, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 647, .adv_w = 144, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 658, .adv_w = 134, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 666, .adv_w = 144, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 677, .adv_w = 136, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 685, .adv_w = 89, .box_w = 5, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 694, .adv_w = 144, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 705, .adv_w = 141, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 714, .adv_w = 62, .box_w = 1, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 716, .adv_w = 61, .box_w = 3, .box_h = 15, .ofs_x = -1, .ofs_y = -3},
    {.bitmap_index = 722, .adv_w = 130, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 733, .adv_w = 62, .box_w = 1, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 735, .adv_w = 224, .box_w = 11, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 748, .adv_w = 141, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 755, .adv_w = 146, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 763, .adv_w = 144, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 774, .adv_w = 146, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 785, .adv_w = 87, .box_w = 4, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 790, .adv_w = 132, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 798, .adv_w = 84, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 805, .adv_w = 141, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 812, .adv_w = 124, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 820, .adv_w = 192, .box_w = 12, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 834, .adv_w = 127, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 843, .adv_w = 121, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 855, .adv_w = 127, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 863, .adv_w = 87, .box_w = 5, .box_h = 15, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 873, .adv_w = 62, .box_w = 1, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 875, .adv_w = 87, .box_w = 5, .box_h = 15, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 885, .adv_w = 174, .box_w = 9, .box_h = 3, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 889, .adv_w = 92, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 891, .adv_w = 0, .box_w = 5, .box_h = 5, .ofs_x = 3, .ofs_y = 13},
    {.bitmap_index = 895, .adv_w = 92, .box_w = 3, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 899, .adv_w = 122, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 908, .adv_w = 109, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 912, .adv_w = 85, .box_w = 5, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 921, .adv_w = 61, .box_w = 3, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 927, .adv_w = 113, .box_w = 5, .box_h = 15, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 937, .adv_w = 61, .box_w = 3, .box_h = 16, .ofs_x = 1, .ofs_y = -5},
    {.bitmap_index = 943, .adv_w = 191, .box_w = 10, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 960, .adv_w = 61, .box_w = 1, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 962, .adv_w = 230, .box_w = 13, .box_h = 9, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 977, .adv_w = 118, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 986, .adv_w = 230, .box_w = 13, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 999, .adv_w = 230, .box_w = 13, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1016, .adv_w = 171, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = -5},
    {.bitmap_index = 1030, .adv_w = 173, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = -5},
    {.bitmap_index = 1044, .adv_w = 173, .box_w = 9, .box_h = 16, .ofs_x = 1, .ofs_y = -5},
    {.bitmap_index = 1062, .adv_w = 124, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1068, .adv_w = 124, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1077, .adv_w = 103, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1082, .adv_w = 103, .box_w = 5, .box_h = 12, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1090, .adv_w = 313, .box_w = 18, .box_h = 9, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1111, .adv_w = 313, .box_w = 18, .box_h = 14, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1143, .adv_w = 326, .box_w = 19, .box_h = 10, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1167, .adv_w = 326, .box_w = 19, .box_h = 14, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1201, .adv_w = 186, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1215, .adv_w = 186, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1229, .adv_w = 166, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = -5},
    {.bitmap_index = 1244, .adv_w = 166, .box_w = 9, .box_h = 17, .ofs_x = 1, .ofs_y = -5},
    {.bitmap_index = 1264, .adv_w = 36, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1265, .adv_w = 231, .box_w = 13, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1285, .adv_w = 189, .box_w = 10, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1302, .adv_w = 246, .box_w = 13, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1320, .adv_w = 180, .box_w = 9, .box_h = 15, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1337, .adv_w = 155, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = -6},
    {.bitmap_index = 1348, .adv_w = 184, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1361, .adv_w = 118, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1367, .adv_w = 113, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1373, .adv_w = 191, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1385, .adv_w = 191, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = -7},
    {.bitmap_index = 1400, .adv_w = 0, .box_w = 4, .box_h = 4, .ofs_x = 3, .ofs_y = 8},
    {.bitmap_index = 1402, .adv_w = 0, .box_w = 5, .box_h = 5, .ofs_x = 2, .ofs_y = 9},
    {.bitmap_index = 1406, .adv_w = 0, .box_w = 4, .box_h = 3, .ofs_x = 3, .ofs_y = -3},
    {.bitmap_index = 1408, .adv_w = 0, .box_w = 4, .box_h = 2, .ofs_x = 2, .ofs_y = 11},
    {.bitmap_index = 1409, .adv_w = 0, .box_w = 4, .box_h = 4, .ofs_x = 2, .ofs_y = 10},
    {.bitmap_index = 1411, .adv_w = 0, .box_w = 4, .box_h = 3, .ofs_x = 3, .ofs_y = -3},
    {.bitmap_index = 1413, .adv_w = 0, .box_w = 4, .box_h = 3, .ofs_x = 2, .ofs_y = 11},
    {.bitmap_index = 1415, .adv_w = 0, .box_w = 3, .box_h = 3, .ofs_x = 3, .ofs_y = 10},
    {.bitmap_index = 1417, .adv_w = 0, .box_w = 5, .box_h = 2, .ofs_x = 1, .ofs_y = 12},
    {.bitmap_index = 1419, .adv_w = 0, .box_w = 3, .box_h = 4, .ofs_x = 3, .ofs_y = 13},
    {.bitmap_index = 1421, .adv_w = 0, .box_w = 3, .box_h = 4, .ofs_x = 3, .ofs_y = -4},
    {.bitmap_index = 1423, .adv_w = 0, .box_w = 5, .box_h = 5, .ofs_x = 2, .ofs_y = 11},
    {.bitmap_index = 1427, .adv_w = 142, .box_w = 5, .box_h = 3, .ofs_x = 2, .ofs_y = 11},
    {.bitmap_index = 1429, .adv_w = 135, .box_w = 5, .box_h = 5, .ofs_x = 2, .ofs_y = 2},
    {.bitmap_index = 1433, .adv_w = 95, .box_w = 2, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1436, .adv_w = 152, .box_w = 7, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1446, .adv_w = 187, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1457, .adv_w = 132, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1466, .adv_w = 157, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1476, .adv_w = 163, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1487, .adv_w = 163, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1498, .adv_w = 163, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1509, .adv_w = 147, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1518, .adv_w = 161, .box_w = 7, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1528, .adv_w = 103, .box_w = 4, .box_h = 8, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1532, .adv_w = 74, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1534, .adv_w = 198, .box_w = 9, .box_h = 8, .ofs_x = 2, .ofs_y = 3},
    {.bitmap_index = 1543, .adv_w = 230, .box_w = 13, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1552, .adv_w = 189, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1564, .adv_w = 0, .box_w = 1, .box_h = 3, .ofs_x = 5, .ofs_y = 12},
    {.bitmap_index = 1565, .adv_w = 69, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 14},
    {.bitmap_index = 1567, .adv_w = 230, .box_w = 13, .box_h = 11, .ofs_x = 1, .ofs_y = -6},
    {.bitmap_index = 1585, .adv_w = 171, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = -5},
    {.bitmap_index = 1599, .adv_w = 111, .box_w = 6, .box_h = 14, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1610, .adv_w = 231, .box_w = 13, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1623, .adv_w = 235, .box_w = 14, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1643, .adv_w = 235, .box_w = 13, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1665, .adv_w = 184, .box_w = 10, .box_h = 8, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1675, .adv_w = 118, .box_w = 6, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1686, .adv_w = 113, .box_w = 5, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1695, .adv_w = 191, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1707, .adv_w = 118, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1713, .adv_w = 135, .box_w = 5, .box_h = 5, .ofs_x = 2, .ofs_y = 2},
    {.bitmap_index = 1717, .adv_w = 95, .box_w = 2, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1720, .adv_w = 152, .box_w = 7, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1730, .adv_w = 187, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1741, .adv_w = 156, .box_w = 7, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1751, .adv_w = 166, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1762, .adv_w = 138, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1772, .adv_w = 163, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1783, .adv_w = 163, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1794, .adv_w = 147, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_1[] = {
    0x0, 0x9, 0xf, 0x13
};

static const uint8_t glyph_id_ofs_list_4[] = {
    0, 0, 0, 1
};

static const uint16_t unicode_list_6[] = {
    0x0, 0xa, 0x12, 0x24, 0x2d, 0x35, 0x3b, 0x46,
    0x4c, 0x56, 0x58, 0x61
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 1548, .range_length = 20, .glyph_id_start = 96,
        .unicode_list = unicode_list_1, .glyph_id_ofs_list = NULL, .list_length = 4, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 1569, .range_length = 26, .glyph_id_start = 100,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 1600, .range_length = 22, .glyph_id_start = 126,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 1623, .range_length = 4, .glyph_id_start = 148,
        .unicode_list = NULL, .glyph_id_ofs_list = glyph_id_ofs_list_4, .list_length = 4, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL
    },
    {
        .range_start = 1632, .range_length = 17, .glyph_id_start = 150,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 1652, .range_length = 98, .glyph_id_start = 167,
        .unicode_list = unicode_list_6, .glyph_id_ofs_list = NULL, .list_length = 12, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 1776, .range_length = 10, .glyph_id_start = 179,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Pair left and right glyphs for kerning*/
static const uint8_t kern_pair_glyph_ids[] =
{
    3, 3,
    3, 8,
    3, 34,
    3, 66,
    3, 68,
    3, 69,
    3, 70,
    3, 72,
    3, 78,
    3, 79,
    3, 80,
    3, 81,
    3, 82,
    3, 84,
    3, 88,
    8, 3,
    8, 8,
    8, 34,
    8, 66,
    8, 68,
    8, 69,
    8, 70,
    8, 72,
    8, 78,
    8, 79,
    8, 80,
    8, 81,
    8, 82,
    8, 84,
    8, 88,
    9, 55,
    9, 56,
    9, 58,
    13, 3,
    13, 8,
    15, 3,
    15, 8,
    16, 16,
    34, 3,
    34, 8,
    34, 32,
    34, 36,
    34, 40,
    34, 48,
    34, 50,
    34, 53,
    34, 54,
    34, 55,
    34, 56,
    34, 58,
    34, 80,
    34, 85,
    34, 86,
    34, 87,
    34, 88,
    34, 90,
    34, 91,
    35, 53,
    35, 55,
    35, 58,
    36, 10,
    36, 53,
    36, 62,
    36, 94,
    37, 13,
    37, 15,
    37, 34,
    37, 53,
    37, 55,
    37, 57,
    37, 58,
    37, 59,
    38, 53,
    38, 68,
    38, 69,
    38, 70,
    38, 71,
    38, 72,
    38, 80,
    38, 82,
    38, 86,
    38, 87,
    38, 88,
    38, 90,
    39, 13,
    39, 34,
    39, 43,
    39, 53,
    39, 66,
    39, 68,
    39, 69,
    39, 70,
    39, 72,
    39, 80,
    39, 82,
    39, 83,
    39, 86,
    39, 87,
    39, 90,
    41, 34,
    41, 53,
    41, 57,
    41, 58,
    42, 34,
    42, 53,
    42, 57,
    42, 58,
    43, 34,
    44, 14,
    44, 36,
    44, 40,
    44, 48,
    44, 50,
    44, 68,
    44, 69,
    44, 70,
    44, 72,
    44, 78,
    44, 79,
    44, 80,
    44, 81,
    44, 82,
    44, 86,
    44, 87,
    44, 88,
    44, 90,
    45, 3,
    45, 8,
    45, 34,
    45, 36,
    45, 40,
    45, 48,
    45, 50,
    45, 53,
    45, 54,
    45, 55,
    45, 56,
    45, 58,
    45, 86,
    45, 87,
    45, 88,
    45, 90,
    46, 34,
    46, 53,
    46, 57,
    46, 58,
    47, 34,
    47, 53,
    47, 57,
    47, 58,
    48, 13,
    48, 15,
    48, 34,
    48, 53,
    48, 55,
    48, 57,
    48, 58,
    48, 59,
    49, 13,
    49, 15,
    49, 34,
    49, 43,
    49, 57,
    49, 59,
    49, 66,
    49, 68,
    49, 69,
    49, 70,
    49, 72,
    49, 80,
    49, 82,
    49, 85,
    49, 87,
    49, 90,
    50, 53,
    50, 55,
    50, 56,
    50, 58,
    51, 53,
    51, 55,
    51, 58,
    53, 13,
    53, 14,
    53, 15,
    53, 34,
    53, 36,
    53, 40,
    53, 43,
    53, 48,
    53, 50,
    53, 52,
    53, 53,
    53, 55,
    53, 56,
    53, 58,
    53, 66,
    53, 68,
    53, 69,
    53, 70,
    53, 72,
    53, 78,
    53, 79,
    53, 80,
    53, 81,
    53, 82,
    53, 83,
    53, 84,
    53, 86,
    53, 87,
    53, 88,
    53, 89,
    53, 90,
    53, 91,
    54, 34,
    55, 10,
    55, 13,
    55, 14,
    55, 15,
    55, 34,
    55, 36,
    55, 40,
    55, 48,
    55, 50,
    55, 62,
    55, 66,
    55, 68,
    55, 69,
    55, 70,
    55, 72,
    55, 80,
    55, 82,
    55, 83,
    55, 86,
    55, 87,
    55, 90,
    55, 94,
    56, 10,
    56, 13,
    56, 14,
    56, 15,
    56, 34,
    56, 53,
    56, 62,
    56, 66,
    56, 68,
    56, 69,
    56, 70,
    56, 72,
    56, 80,
    56, 82,
    56, 83,
    56, 86,
    56, 94,
    57, 14,
    57, 36,
    57, 40,
    57, 48,
    57, 50,
    57, 55,
    57, 68,
    57, 69,
    57, 70,
    57, 72,
    57, 80,
    57, 82,
    57, 86,
    57, 87,
    57, 90,
    58, 7,
    58, 10,
    58, 11,
    58, 13,
    58, 14,
    58, 15,
    58, 34,
    58, 36,
    58, 40,
    58, 43,
    58, 48,
    58, 50,
    58, 52,
    58, 53,
    58, 54,
    58, 55,
    58, 56,
    58, 57,
    58, 58,
    58, 62,
    58, 66,
    58, 68,
    58, 69,
    58, 70,
    58, 71,
    58, 72,
    58, 78,
    58, 79,
    58, 80,
    58, 81,
    58, 82,
    58, 83,
    58, 84,
    58, 85,
    58, 86,
    58, 87,
    58, 89,
    58, 90,
    58, 91,
    58, 94,
    59, 34,
    59, 36,
    59, 40,
    59, 48,
    59, 50,
    59, 68,
    59, 69,
    59, 70,
    59, 72,
    59, 80,
    59, 82,
    59, 86,
    59, 87,
    59, 88,
    59, 90,
    60, 43,
    60, 54,
    66, 3,
    66, 8,
    66, 87,
    66, 90,
    67, 3,
    67, 8,
    67, 87,
    67, 89,
    67, 90,
    67, 91,
    68, 3,
    68, 8,
    70, 3,
    70, 8,
    70, 87,
    70, 90,
    71, 3,
    71, 8,
    71, 10,
    71, 62,
    71, 68,
    71, 69,
    71, 70,
    71, 72,
    71, 82,
    71, 94,
    73, 3,
    73, 8,
    76, 68,
    76, 69,
    76, 70,
    76, 72,
    76, 82,
    78, 3,
    78, 8,
    79, 3,
    79, 8,
    80, 3,
    80, 8,
    80, 87,
    80, 89,
    80, 90,
    80, 91,
    81, 3,
    81, 8,
    81, 87,
    81, 89,
    81, 90,
    81, 91,
    83, 3,
    83, 8,
    83, 13,
    83, 15,
    83, 66,
    83, 68,
    83, 69,
    83, 70,
    83, 71,
    83, 72,
    83, 80,
    83, 82,
    83, 85,
    83, 87,
    83, 88,
    83, 90,
    85, 80,
    87, 3,
    87, 8,
    87, 13,
    87, 15,
    87, 66,
    87, 68,
    87, 69,
    87, 70,
    87, 71,
    87, 72,
    87, 80,
    87, 82,
    88, 13,
    89, 68,
    89, 69,
    89, 70,
    89, 72,
    89, 80,
    89, 82,
    90, 3,
    90, 8,
    90, 13,
    90, 15,
    90, 66,
    90, 68,
    90, 69,
    90, 70,
    90, 71,
    90, 72,
    90, 80,
    90, 82,
    91, 68,
    91, 69,
    91, 70,
    91, 72,
    91, 80,
    91, 82,
    92, 43,
    92, 54,
    101, 172,
    101, 173,
    116, 100,
    116, 101,
    116, 102,
    116, 105,
    116, 106,
    116, 107,
    116, 108,
    116, 109,
    116, 110,
    116, 114,
    116, 115,
    116, 118,
    116, 119,
    116, 120,
    116, 121,
    116, 122,
    116, 123,
    116, 126,
    116, 127,
    116, 128,
    116, 129,
    116, 130,
    116, 131,
    116, 132,
    116, 133,
    116, 135,
    116, 136,
    116, 168,
    116, 172,
    116, 173,
    117, 100,
    117, 101,
    117, 102,
    117, 105,
    117, 106,
    117, 107,
    117, 108,
    117, 109,
    117, 110,
    117, 114,
    117, 115,
    117, 118,
    117, 119,
    117, 120,
    117, 121,
    117, 122,
    117, 123,
    117, 126,
    117, 127,
    117, 128,
    117, 129,
    117, 130,
    117, 131,
    117, 132,
    117, 133,
    117, 135,
    117, 136,
    117, 168,
    117, 172,
    117, 173,
    170, 100,
    170, 101,
    170, 102,
    170, 105,
    170, 106,
    170, 107,
    170, 108,
    170, 109,
    170, 110,
    170, 114,
    170, 115,
    170, 118,
    170, 119,
    170, 120,
    170, 121,
    170, 122,
    170, 123,
    170, 126,
    170, 127,
    170, 128,
    170, 129,
    170, 130,
    170, 131,
    170, 132,
    170, 133,
    170, 135,
    170, 136,
    170, 168,
    170, 172,
    170, 173
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static const int8_t kern_pair_values[] =
{
    -13, -13, -15, -6, -7, -7, -7, -7,
    -2, -2, -8, -2, -7, -10, 1, -13,
    -13, -15, -6, -7, -7, -7, -7, -2,
    -2, -8, -2, -7, -10, 1, 3, 2,
    3, -21, -21, -21, -21, -28, -15, -15,
    -8, -1, -1, -1, -1, -16, -2, -11,
    -9, -12, -1, -2, -1, -6, -4, -6,
    2, -3, -3, -7, -3, -4, -1, -2,
    -13, -13, -3, -3, -3, -3, -5, -3,
    3, -2, -2, -2, -2, -2, -2, -2,
    -2, -3, -3, -3, -29, -21, -33, 3,
    -4, -3, -3, -3, -3, -3, -3, -3,
    -3, -3, -3, 2, -4, 2, -3, 2,
    -4, 2, -3, -3, -8, -4, -4, -4,
    -4, -3, -3, -3, -3, -3, -3, -3,
    -3, -3, -3, -5, -8, -5, -42, -42,
    2, -8, -8, -8, -8, -34, -7, -22,
    -18, -30, -5, -17, -11, -17, 2, -4,
    2, -3, 2, -4, 2, -3, -13, -13,
    -3, -3, -3, -3, -5, -3, -40, -40,
    -17, -25, -4, -3, -1, -2, -2, -2,
    -2, -2, -2, 2, 2, 2, -5, -3,
    -2, -4, -10, -2, -6, -27, -29, -27,
    -10, -3, -3, -30, -3, -3, -2, 2,
    2, 2, 2, -14, -12, -12, -12, -12,
    -14, -14, -12, -14, -12, -9, -14, -12,
    -9, -7, -10, -9, -7, -3, 3, -28,
    -5, -28, -9, -2, -2, -2, -2, 2,
    -6, -5, -5, -5, -5, -6, -5, -4,
    -3, -1, -1, 2, 2, -15, -7, -15,
    -5, 2, 2, -4, -4, -4, -4, -4,
    -4, -4, -3, -2, 2, -6, -3, -3,
    -3, -3, 2, -3, -3, -3, -3, -3,
    -3, -3, -4, -4, -4, 3, -6, -26,
    -6, -26, -12, -4, -4, -12, -4, -4,
    -2, 2, -12, 2, 2, 2, 2, 2,
    -9, -8, -8, -8, -3, -8, -5, -5,
    -8, -5, -8, -5, -7, -3, -5, -2,
    -3, -2, -4, 2, 2, -3, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -2,
    -3, -3, -3, -2, -2, -8, -8, -2,
    -2, -4, -4, -1, -2, -1, -2, -1,
    -1, -2, -2, -2, -2, 2, 2, 3,
    2, -3, -3, -3, -3, -3, 2, -13,
    -13, -2, -2, -2, -2, -2, -13, -13,
    -13, -13, -17, -17, -2, -3, -2, -2,
    -4, -4, -1, -2, -1, -2, 2, 2,
    -15, -15, -5, -2, -2, -2, 2, -2,
    -2, -2, 6, 2, 2, 2, -2, 2,
    2, -13, -13, -2, -2, -2, -2, 2,
    -2, -2, -2, -15, -2, -2, -2, -2,
    -2, -2, 2, 2, -13, -13, -2, -2,
    -2, -2, 2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, 10, 10,
    -22, -18, -18, -7, -18, -22, -22, -22,
    -22, -22, -22, -22, -22, -22, -22, -22,
    -22, -22, -22, -7, -18, -7, -22, -7,
    -22, -7, -7, -22, -28, -28, -22, -18,
    -18, -7, -18, -22, -22, -22, -22, -22,
    -22, -22, -22, -22, -22, -22, -22, -22,
    -22, -7, -18, -7, -22, -7, -22, -7,
    -7, -22, -28, -28, -22, -14, -14, -7,
    -16, -14, -22, -14, -14, -22, -22, -22,
    -22, -22, -22, -22, -22, -22, -14, -7,
    -13, -7, -22, -7, -22, -7, -7, -14,
    -27, -27
};

/*Collect the kern pair's data in one place*/
static const lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_glyph_ids,
    .values = kern_pair_values,
    .pair_cnt = 522,
    .glyph_ids_size = 0
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_pairs,
    .kern_scale = 16,
    .cmap_num = 8,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t ui_font_Font1 = {
#else
lv_font_t ui_font_Font1 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 25,          /*The maximum line height required by the font*/
    .base_line = 7,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -4,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if UI_FONT_FONT1*/

