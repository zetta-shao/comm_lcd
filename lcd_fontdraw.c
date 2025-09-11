#include "lcd_fontdraw.h"
#include "lcd_fonts.h"
#include <stdio.h>
#if _aryfont_
dotfont_t aryfont[] = {
#ifdef LCDFONT_INCLUDE_FONT_5x7
    &Font_5x7, //0
#endif
#ifdef LCDFONT_INCLUDE_FONT_6x8
    &Font_6x8, //1
#endif
#ifdef LCDFONT_INCLUDE_FONT_6x12
    &Font_6x12, //2
#endif
#ifdef LCDFONT_INCLUDE_FONT_7x10
    &Font_7x10, //3
#endif
#ifdef LCDFONT_INCLUDE_FONT_8x16
    &Font_8x16, //4
#endif
#ifdef LCDFONT_INCLUDE_FONT_11x18
    &Font_11x18, //5
#endif
#ifdef LCDFONT_INCLUDE_FONT_12x24
    &Font_12x24, //6
#endif
#ifdef LCDFONT_INCLUDE_FONT_16x21N
    &Font_16x21N, //7
#endif
#ifdef LCDFONT_INCLUDE_FONT_16x24
    &Font_16x24, //8
#endif
#ifdef LCDFONT_INCLUDE_FONT_16x26
    &Font_16x26, //9
#endif
#ifdef LCDFONT_INCLUDE_FONT_16x32
    &Font_16x32, //10
#endif
    NULL,
};
#endif
uint8_t fontdraw_getpow(uint8_t val) {
    uint8_t res = 0x80; uint8_t i, m=0x7f;
    for(i=7; res!=0; res>>=1, i--, m>>=1) {
        if(val&res) {
            if(val & m) return val; else return i;
        }
    } return 0;
}

void fontdraw_drawpixelBW(fontdraw_t *d, uint32_t x, uint32_t y, int8_t color) {
    int ox = (x & d->posmask) ^ d->invposmask;
    int oy = (y & d->posmask) ^ d->invposmask;
    //if(d->flags & FONTDRAW_WIDTHPREDIV) x >>= d->widthprediv;
    if(d->flags & FONTDRAW_WIDTHPREDIV) x &= ~(d->posmask);
    //if(d->flags & FONTDRAW_HEIGHTPREDIV) y >>= d->heightprediv;
    if(d->flags & FONTDRAW_HEIGHTPREDIV) y &= ~(d->posmask);
    if(d->flags & FONTDRAW_HEIGHTDIV) y >>= d->heightScale;
    else if(d->flags & FONTDRAW_HEIGHTMUL) { y <<= d->heightScale; }
    //else if(d->heightScale!=0) { y /= d->heightScale; }
    if(d->flags & FONTDRAW_WIDTHDIV) x >>= d->widthScale;
    else if(d->flags & FONTDRAW_WIDTHMUL) x <<= d->widthScale;
    //else if(d->widthScale!=0) { x /= d->widthScale; }
    if(d->flags & FONTDRAW_VERTICALDRAW) { 
        if(color) d->pFrameBuf[x+y] |= 1 << oy;
        else d->pFrameBuf[x+y] &= ~(1 << oy);
    } else {
        if(color) d->pFrameBuf[x+y] |= 1 << ox;
        else d->pFrameBuf[x+y] &= ~(1 << ox); 
    }
}


char fontdraw_dot_Font(fontdraw_t *d, int8_t color, struct FontDef *font) {
    int i, j, posx=d->curX, posy=d->curY, mask=0, ch = '.';
    mask = font->dotmask;

    //if(font->flags & FONT_FLAG_BTAB) { ch += 31; }
    ch -= font->index_star;
    if(font->flags & FONT_FLAG_VERT) { ch *= font->FontWidth; }
    else { ch *= font->FontHeight; }
    if(font->flags & FONT_FLAG_WPTR) {
        uint16_t t, m, *px = ch + (uint16_t*)font->data;
        if(font->flags & FONT_FLAG_VERT) {
            for(i=0; i<font->FontWidth; i++, posx++) {
                //if(! px[i]) continue;
                for(m=font->mask,j=posy,t=px[i]; m!=0; m>>=1) {
                    if(! (m & mask)) continue;
                    d->pixeldraw(d, posx, j++, (t & m) ? color : !color);
                } }
        } else {
            for(i=0; i<font->FontHeight; i++, posy++) {
                //if(! px[i]) continue;
                for(m=font->mask,j=posx,t=px[i]; m!=0; m>>=1) {
                    if(! (m & mask)) continue;
                    d->pixeldraw(d, j++, posy, (t & m) ? color : !color);
                } }
        }
    } else {
        uint8_t t, m, *px = ch + (uint8_t*)font->data;
        if(font->flags & FONT_FLAG_VERT) {
            for(i = 0; i < font->FontWidth; i++, posx++) {
                //if(! px[i]) continue;
                for(m=font->mask,j=posy,t=px[i]; m!=0; m>>=1) {
                    if(! (m & mask)) continue;
                    d->pixeldraw(d, posx, j++, (t & m) ? color : !color);
                } }
        } else {
            for(i=0; i<font->FontHeight; i++, posy++) {
                //if(! px[i]) continue;
                for(m=font->mask,j=posx,t=px[i]; m!=0; m>>=1) {
                    if(! (m & mask)) continue;
                    d->pixeldraw(d, j++, posy, (t & m) ? color : !color);
                } }
        }
    }

    if(d->flags & FONTDRAW_STRVERTICAL) { d->curY += font->dotwidth; }
    else { d->curX += font->dotwidth; }
    return 0;
}

//char fontdraw_char(fontdraw_t *d, void *pFontDef, int posx, int posy, uint8_t ch, int color) {
char fontdraw_charFont(fontdraw_t *d, uint8_t ch, int8_t color, void *pvFontDef) {
    int i, j, posx=d->curX, posy=d->curY, chr = ch;

    FontDef *font = (FontDef*)pvFontDef;
    if(ch == 0) return 0;
    if(ch < font->index_star || ch > font->index_end) return 0;
    if( d->frameWidth < (posx + font->FontWidth) ||
        d->frameHeight < (posy + font->FontHeight)) { return 0; }
    if(ch == '.') return fontdraw_dot_Font(d, color, pvFontDef);

    //if(font->flags & FONT_FLAG_BTAB) { chr -= 1; } else { chr -= 32; }
    chr -= font->index_star;
    if(font->flags & FONT_FLAG_VERT) { chr *= font->FontWidth; }
    else { chr *= font->FontHeight; }
    if(font->flags & FONT_FLAG_WPTR) {
        uint16_t t, m, *px = chr + (uint16_t*)font->data;
        if(font->flags & FONT_FLAG_VERT) {
            for(i = 0; i < font->FontWidth; i++, posx++) {
                for(m=font->mask,j=posy,t=px[i]; m!=0; j++, m>>=1) {
                    d->pixeldraw(d, posx, j, (t & m) ? color : !color);
                } }
        } else {
            for(i = 0; i < font->FontHeight; i++, posy++) {
                for(m=font->mask,j=posx,t=px[i]; m!=0; j++, m>>=1) {
                    d->pixeldraw(d, j, posy, (t & m) ? color : !color);
                } }
        }
    } else {
        uint8_t t, m, *px = chr + (uint8_t*)font->data;
        if(font->flags & FONT_FLAG_VERT) {
            for(i = 0; i < font->FontWidth; i++, posx++) {
                for(m=font->mask,j=posy,t=px[i]; m!=0; j++, m>>=1) {
                    d->pixeldraw(d, posx, j, (t & m) ? color : !color);
                } }
        } else {
            for(i = 0; i < font->FontHeight; i++, posy++) {
                for(m=font->mask,j=posx,t=px[i]; m!=0; j++, m>>=1) {
                    d->pixeldraw(d, j, posy, (t & m) ? color : !color);
                } }
        }
    }
    if(d->flags & FONTDRAW_STRVERTICAL) { d->curY += font->FontHeight; }
    else { d->curX += font->FontWidth; }
    return ch;
}

char fontdraw_char(fontdraw_t *d, uint8_t ch) { return fontdraw_charFont(d, ch, 0, d->pFont); }
char fontdraw_charC(fontdraw_t *d, uint8_t ch, int8_t color) { return fontdraw_charFont(d, ch, color, d->pFont); }

void fontdraw_string(fontdraw_t *d, char *s) {
    while(*s) { fontdraw_charFont(d, *s, 0, d->pFont); s++; }
}

void fontdraw_stringC(fontdraw_t *d, char *s, int8_t color) {
    while(*s) { fontdraw_charFont(d, *s, color, d->pFont); s++; }
}

void fontdraw_stringFont(fontdraw_t *d, char *s, int8_t color, void *pvFontDef) {
    while(*s) { fontdraw_charFont(d, *s, color, pvFontDef); s++; }
}

void fontdraw_setpos(fontdraw_t *d, uint32_t x, uint32_t y) { d->curX = x; d->curY = y; }

void fontdraw_setColor(fontdraw_t *d, int colorf, int colorb) { d->colorf=colorf; d->colorb=colorb; }

void fontdraw_fill(fontdraw_t *d, int8_t color) {
    if(!d->fill_color) return;
    //memset(d->pFrameBuf, color, (d->frameWidth*d->frameHeight)/8);
    d->fill_color(d->parent, color);
}

void fontdraw_set_string_dir(fontdraw_t *d, int vert) { if(vert) d->flags|=FONTDRAW_STRVERTICAL; else d->flags &= ~FONTDRAW_STRVERTICAL; }

int8_t str_3digitL(int32_t val, char *outstr) { //3+1
    int8_t res = (val < 0) ? 0 : 1;
    val = (val > 0) ? val : -val;
    if(val < 1000) {
        sprintf(outstr, ".%03d", (int)val);
    } else if(val < 10000) {
        sprintf(outstr, "%01d.%02d", (int)(val/1000)%10, (int)(val%1000)/10);
    } else if(val < 100000) {
        sprintf(outstr, "%02d.%01d", (int)(val/1000)%100, (int)(val%1000)/100);
    } else if(val < 1000000) {
        sprintf(outstr, "%03d", (int)(val/1000)%1000);
    }
    outstr[4] = 0;
    return res;
}

int8_t str_3digitU(int32_t val, char *outstr, char *unit) {
    int8_t res = str_3digitL(val, outstr);
    strcat(outstr, unit);
    return res;
}

int8_t str_4digitL(int32_t val, char *outstr) { //4+1
    int8_t res = (val < 0) ? 0 : 1;
    if(val < 1000) {
        sprintf(outstr, ".%03ld0", val);
    } else if(val < 10000) {
        sprintf(outstr, "%1ld.%03ld", (val/1000)%10, val%1000);
    } else if(val < 100000) {
        sprintf(outstr, "%2ld.%02ld", (val/1000)%100, (val/10)%100);
    } else if(val < 1000000) {
        sprintf(outstr, "%3ld.%01ld", (val/1000)%1000, (val/100)%10);
    } else if(val < 10000000) {
        sprintf(outstr, "%4ld.", (val/1000)%10000);
    }
    outstr[5] = 0;
    return res;
}

int8_t str_4digitU(int32_t val, char *outstr, char *unit) { //4+1
    int8_t res = str_4digitL(val, outstr);
    strcat(outstr, unit);
    return res;
}

int8_t str_5digit(int16_t val, char *outstr) {
    int8_t res = (val < 0) ? 0 : 1;
#ifdef ICO_NUMBER_DOT
    if(val < 10000)
        sprintf(outstr, "  %03d", val%1000);
    else
        sprintf(outstr, "%01d %03d", (val/10000)%10, val%1000);

    outstr[1] = ((val/1000)%10) + ICO_NUMBER_DOT;
    outstr[5] = 0;
#else
    (void)val; (void)outstr;
#endif
    return res;
}

typedef int8_t (*__strfmt)(int16_t, char*);
#ifdef _aryfont_
dotfont_t* find_dotfont(FontDef *font) {
    dotfont_t *pD = aryfont;
    for(; pD->font; pD++) {
        if(pD->font != font) continue;
        return pD;
    }
    return NULL;
}
#endif
