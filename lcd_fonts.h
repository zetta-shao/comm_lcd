#include <stdint.h>

#ifndef __LCDFONT_FONTS_H__
#define __LCDFONT_FONTS_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "lcd_font_sel.h"
#include "lcd_fontdraw.h"

#define FONT_FLAG_WPTR    0x01 //16bit ptr.
#define FONT_FLAG_VERT    0x02 
#define FONT_FLAG_BTAB    0x04 //bit table.

typedef struct FontDef FontDef;

struct FontDef {
    const uint8_t FontWidth;    /*!< Font width in pixels */
    const uint8_t FontHeight;   /*!< Font height in pixels */
    const uint8_t index_star;
    const uint8_t index_end;
    const uint16_t flags;
    const uint8_t unused;
    const uint8_t dotwidth;
    const uint32_t dotmask;
    const uint32_t mask; //for front pixel draw.
    const uint16_t *data; /*!< Pointer to data font data array */
};

#ifndef dotfont_t
#endif

//font8.c
extern FontDef Font_5x7; //5x7 no padding
extern FontDef Font_6x8;
extern FontDef Font_6x12;
extern FontDef Font_7x10;
extern FontDef Font_8x16;
//fonts.c
extern FontDef Font_11x18;
extern FontDef Font_16x21N;
extern FontDef Font_16x26;
extern FontDef Font_16x24;
extern FontDef Font_12x16N;
extern FontDef Font_12x24;
extern FontDef Font_16x32;

#ifdef LCDFONT_INCLUDE_FONT_6x8
#define ICO_NUMBER_DOT 1 //total 10
#define ICO_DOT_NUMBER 140 //total 10
#define ICO_BATTERY_VOLTAGE 11 //total 5
#define ICO_VOUTVOL 16 //total 15
#define ICO_BATUM 127 //total 2
#define ICO_VOUTNUM 129 //total 8
#define ICO_PWRIN 137
#define ICO__BAT 138
#define ICO__USB 139
#endif

#define FONT_SHIFT_NUMBER   48

#ifdef __cplusplus
}
#endif
#endif // __LCDFONT_FONTS_H__
