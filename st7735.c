#include "st7735.h"

const uint16_t st7735_colorpalette[] = {
    WHITE,BLUE,BRED,GRED,GBLUE,RED, MAGENTA,GREEN,CYAN,YELLOW,BROWN,GRAY,LIGHTBLUE,LIGHTGREEN
};

void st7735_window(st7735_t *d, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
#if     ((USE_HORIZONTAL == 0) || (USE_HORIZONTAL == 1))
    st7735_w_reg(d, 0x2a);//�е�����
    st7735_write16(d, x1+26);
    st7735_write16(d, x2+26);
    st7735_w_reg(d, 0x2b);//�е�����
    st7735_write16(d, y1+1);
    st7735_write16(d, y2+1);
    st7735_w_reg(d, 0x2c);//������д
#else
    st7735_w_reg(d, 0x2a);//�е�����
    st7735_write16(d, x1+1);
    st7735_write16(d, x2+1);
    st7735_w_reg(d, 0x2b);//�е�����
    st7735_write16(d, y1+26);
    st7735_write16(d, y2+26);
    st7735_w_reg(d, 0x2c);//������д
#endif
}

void st7735_fill(st7735_t *d, uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color) {
	uint16_t x, y;
    if(xend > d->d.frameWidth) xend = d->d.frameWidth;
    if(yend > d->d.frameHeight) yend = d->d.frameHeight;
    st7735_window(d, xsta, ysta, xend-1, yend-1);
    for(y=ysta; y<yend; y++) {
        for(x=xsta; x<xend; x++) { st7735_write16(d, color); } }
}

void st7735_fillscreen(lcddev_t *d, uint8_t color) {
#ifdef __AMD64
    memset(d->pFrameBuf, color, d->dwFrameByteSize);
#else
    st7735_fill(d->parent, 0, 0, d->frameWidth, d->frameHeight, color);
#endif
}

void st7735_drawPoint(st7735_t *d, uint16_t x, uint16_t y, uint16_t color) {
//#ifdef __STM32
    const uint8_t mmsk[8] = { 0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80 };
    const uint8_t umsk[8] = { 0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f };
    int of = y * d->d.oneLineOffsetSize;
    of += (x >> 3); x &= 7;
    if(color) d->d.pFrameBuf[of] |= mmsk[x];
    else d->d.pFrameBuf[of] &= umsk[x];
#if 0
	st7735_window(d, x, y, x, y);
	st7735_write16(d, color);
#endif
}

void st7735_pixeldraw(lcddev_t *d, uint16_t x, uint16_t y, uint8_t color) {
    st7735_drawPoint((st7735_t*)d->parent, x, y, (color==0)?d->colorb:d->colorf);
}

void st7735_update(lcddev_t *d) {
    if(d->flags & FONTDRAW_IDLE) return;
#ifdef __AMD64
    (void)d;
#else
    int x, y, m, l;
    st7735_t *q = d->parent;
    uint8_t *px, pt;
    uint16_t clrf, clrb;
    st7735_window(q, 0, 0, d->frameWidth-1, d->frameHeight-1);
    px = d->pFrameBuf;
    clrf = d->colorf; clrb = d->colorb;
    for(y=0; y<d->frameHeight; y++, px+=d->oneLineOffsetSize) {
        for(x=0, l=0; x<d->frameWidth; l++) {
            pt = px[l];
            for(m=1; m<129 && x<d->frameWidth; m <<= 1, x++) {
                if(pt & m) st7735_write16(q, clrf);
                else st7735_write16(q, clrb); } } }
#endif
}

void st7735_update_window(st7735_t *q, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    int i, j, m, l;
    uint8_t *px, pt;
    uint16_t clrf, clrb;

    if(q->d.flags & FONTDRAW_IDLE) return;
    px = q->d.pFrameBuf;
    px += y * q->d.oneLineOffsetSize;
    clrf = q->d.colorf;
    l = (x >> 3);
    clrb = q->d.colorb;
    m = 1 << (x & 7);
    st7735_window(q, x, y, x+w-1, y+h-1);
    for(i=0; i<h; i++, px+=q->d.oneLineOffsetSize) {
        for(j=0; j<w; l++) {
            pt = px[l];
            for(; m<129 && j<w; m<<=1, j++) {
                if(pt & m) { st7735_write16(q, clrf); }
                else { st7735_write16(q, clrb); } }
            m = 1;
        } l = (x >> 3); m = 1 << (x & 7); }
}


void st7735_drawLine(st7735_t *d, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	int i, xerr=0, yerr=0, delta_x=0, delta_y=0, distance=0, incx=0, incy=0, uRow=x1, uCol=y1;	
    if(x1!=x2) { delta_x=abs(x2-x1); incx=(x2>x1)?1:-1; }
    if(y1!=y2) { delta_y=abs(y2-y1); incy=(y2>y1)?1:-1; }
	distance = __max(delta_x, delta_y);
	for(i=0; i<=distance; i++) {
		st7735_drawPoint(d, uRow, uCol, color);
		xerr += delta_x;
		yerr += delta_y;
		if(xerr > distance) { xerr -= distance; uRow+=incx; }
		if(yerr > distance) { yerr -= distance; uCol+=incy; }
	}
}

void st7735_drawRectangle(st7735_t *d, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color) {
	st7735_drawLine(d, x1, y1, x2, y1, color);
	st7735_drawLine(d, x1, y1, x1, y2, color);
	st7735_drawLine(d, x1, y2, x2, y2, color);
	st7735_drawLine(d, x2, y1, x2, y2, color);
}

void st7735_drawCircle(st7735_t *d, uint16_t x0,uint16_t y0,uint8_t r,uint16_t color) {
	int a,b;
	a=0;b=r;	  
    while(a<=b) {
        st7735_drawPoint(d, x0-b,y0-a,color);             //3
        st7735_drawPoint(d, x0+b,y0-a,color);             //0
        st7735_drawPoint(d, x0-a,y0+b,color);             //1
        st7735_drawPoint(d, x0-a,y0-b,color);             //2
        st7735_drawPoint(d, x0+b,y0+a,color);             //4
        st7735_drawPoint(d, x0+a,y0-b,color);             //5
        st7735_drawPoint(d, x0+a,y0+b,color);             //6
        st7735_drawPoint(d, x0-b,y0+a,color);             //7
		a++;
        if((a*a+b*b)>(r*r)) { b--; }
	}
}

void st7735_drawChar(st7735_t *d, char val, FontDef *pF) {
    int x, y, j, oft;
    lcddev_t *t = &d->d;
	//d->d.pFont->FontWidth;
	if(!d->d.pFont) return;
    if((val < pF->index_star) || (val > pF->index_end)) return;
	pF = d->d.pFont;
    oft = val - pF->index_star;
	if(pF->flags & FONT_FLAG_VERT) { oft *= pF->FontWidth; } 
	else { oft *= pF->FontHeight; }
	
	st7735_window(d, d->d.curX, d->d.curY, d->d.curX+pF->FontWidth-1, d->d.curY+pF->FontHeight-1);
	if(pF->flags & FONT_FLAG_WPTR) {
		uint16_t _t, *pT = oft + (uint16_t*)pF->data;
		if(pF->flags &  FONT_FLAG_VERT) {
			for(y=0, j=pF->mask; y<pF->FontHeight; y++, j>>=1) {
				for(x=0; y<pF->FontWidth; x++) {
                    st7735_write16(d, (pT[x]&j)?t->colorf:t->colorb); } }
		} else {
			for(y=0; y<pF->FontHeight; y++) {
				for(x=0, j=pF->mask, _t=pT[y]; x<pF->FontWidth; x++, j>>=1) {
                    st7735_write16(d, (_t&j)?t->colorf:t->colorb); } }
		}
	} else {
		uint8_t _t, *pT = oft + (uint8_t*)pF->data;
		if(pF->flags &  FONT_FLAG_VERT) {
			for(y=0, j=pF->mask; y<pF->FontHeight; y++, j>>=1) {
				for(x=0; y<pF->FontWidth; x++) {
                    st7735_write16(d, (pT[x]&j)?t->colorf:t->colorb); } }
		} else {
			for(y=0; y<pF->FontHeight; y++) {
				for(x=0, j=pF->mask, _t=pT[y]; x<pF->FontWidth; x++, j>>=1) {
                    st7735_write16(d, (_t&j)?t->colorf:t->colorb); } }
		}
	}
    if(d->d.flags & FONTDRAW_STRVERTICAL) { d->d.curY += pF->FontHeight; }
	else { d->d.curX += (pF->FontWidth); }
}

void st7735_setcur(st7735_t *d, uint16_t x, uint16_t y) {
	d->d.curX = x; d->d.curY = y;
}

void st7735_setcolor(st7735_t *d, uint16_t color_front, uint16_t color_back) {
    d->d.colorf = color_front; d->d.colorb=color_back;
}

void st7735_gpioinit(st7735_t *d, swgpio_t *cs, swgpio_t *dc, swgpio_t *rst, swgpio_t *blk) {
    if(!d) return;
    d->blk = NULL; d->cs = NULL; d->dc = NULL; d->rst = NULL;
    d->d.curX = 0;
    d->d.curY = 0;
    if(cs) { d->cs=cs; swgp_gpmode(cs, 1); }
    if(dc) { d->dc=dc; swgp_gpmode(dc, 1); }
    if(rst) { d->rst=rst; swgp_gpmode(rst, 1); }
    if(blk) { d->blk=blk; swgp_gpmode(blk, 1); }
}

void st7735_w_reg(st7735_t *d, uint8_t val) {
    swgp_gpo(d->dc, 0); st7735_write8(d, val); swgp_gpo(d->dc, 1); }

void st7735_write8(st7735_t *d, uint8_t val) {
    swgp_gpo(d->cs, 0); swspi_write(d->dev, &val, 1); swgp_gpo(d->cs, 1);
}

void st7735_write16(st7735_t *d, uint16_t val) { uint8_t *p=(uint8_t*)&val;
    swgp_gpo(d->cs, 0); swspi_write(d->dev, p+1, 1); swspi_write(d->dev, p, 1); swgp_gpo(d->cs, 1);
}

void st7735_reset(st7735_t *d) {
    swgp_gpo(d->cs, 0);
    swgp_gpo(d->rst, 0);
    swgp_delay_ms(100);
    swgp_gpo(d->rst, 1);
    swgp_delay_ms(100);
    swgp_gpo(d->cs, 1);
}

void st7735_set_idle(st7735_t *d, uint8_t val) {
    if(!d) return;
    if(val) {
        d->d.flags |= FONTDRAW_IDLE;
        swgp_gpo(d->blk, 1);
    } else {
        d->d.flags &= ~FONTDRAW_IDLE;
        swgp_gpo(d->blk, 0);
    }
}

void st7735_init(st7735_t *d, uint16_t width, uint16_t height, void *pvport, void *pvFontDef) {
    d->dev = pvport;
    d->d.parent = d;
    d->d.frameWidth = width;
    d->d.frameHeight = height;
    d->d.oneLineOffsetSize = (width >> 3);
    d->d.dwFrameByteSize = d->d.oneLineOffsetSize * d->d.frameHeight;
    d->d.flags = 0;
    d->d.pFrameBuf = d->fbbuf; //(uint8_t*)malloc(d->d.dwFrameByteSize);
#ifdef __AMD64
    //d->d.flags |= FONTDRAW_BIT16;
    d->blk = NULL; d->rst = NULL; d->cs = NULL; d->dc = NULL;
#endif
    d->d.pFont = pvFontDef;
    d->d.curX = 0;
    d->d.curY = 0;
    d->d.colorf = 0xffff;
    d->d.colorb = 0;
    d->d.fill_color = &st7735_fillscreen;
    d->d.pixeldraw = &st7735_pixeldraw;
    d->d.update = &st7735_update;
    swgp_gpo(d->blk, 1);
    st7735_reset(d);
    swgp_gpo(d->blk, 0);
    swgp_gpo(d->dc, 1);

    st7735_w_reg(d, 0x11);//sleep-out
    swgp_delay_ms(120);
    st7735_w_reg(d, 0xb1);//normal-mode
    st7735_write8(d, 0x05);
    st7735_write8(d, 0x3c);
    st7735_write8(d, 0x3c);
    st7735_w_reg(d, 0xb2);//idle-mode
    st7735_write8(d, 0x05);
    st7735_write8(d, 0x3c);
    st7735_write8(d, 0x3c);
    st7735_w_reg(d, 0xb3);//partical-mode
    st7735_write8(d, 0x05);
    st7735_write8(d, 0x3c);
    st7735_write8(d, 0x3c);
    st7735_write8(d, 0x05);
    st7735_write8(d, 0x3c);
    st7735_write8(d, 0x3c);
    st7735_w_reg(d, 0xb4);//dot-inversed
    st7735_write8(d, 0x03);
    st7735_w_reg(d, 0xc0);//avdd-gvdd
    st7735_write8(d, 0xab);
    st7735_write8(d, 0x0b);
    st7735_write8(d, 0x04);
    st7735_w_reg(d, 0xc1);//vgh-vgl
    st7735_write8(d, 0xc5);
    st7735_w_reg(d, 0xc2);//normal-mode
    st7735_write8(d, 0x0d);
    st7735_write8(d, 0x00);
    st7735_w_reg(d, 0xc3);//idle
    st7735_write8(d, 0x8d);
    st7735_write8(d, 0x6a);
    st7735_w_reg(d, 0xc4);//partical+full
    st7735_write8(d, 0x8d);
    st7735_write8(d, 0xee);
    st7735_w_reg(d, 0xc5);//vcom
    st7735_write8(d, 0x0f);
    st7735_w_reg(d, 0xe0);//positive gamma
    st7735_write8(d, 0x07);
    st7735_write8(d, 0x0e);
    st7735_write8(d, 0x08);
    st7735_write8(d, 0x07);
    st7735_write8(d, 0x10);
    st7735_write8(d, 0x07);
    st7735_write8(d, 0x02);
    st7735_write8(d, 0x07);
    st7735_write8(d, 0x09);
    st7735_write8(d, 0x0f);
    st7735_write8(d, 0x25);
    st7735_write8(d, 0x36);
    st7735_write8(d, 0x00);
    st7735_write8(d, 0x08);
    st7735_write8(d, 0x04);
    st7735_write8(d, 0x10);
    st7735_w_reg(d, 0xe1);//negative gamma
    st7735_write8(d, 0x0a);
    st7735_write8(d, 0x0d);
    st7735_write8(d, 0x08);
    st7735_write8(d, 0x07);
    st7735_write8(d, 0x0f);
    st7735_write8(d, 0x07);
    st7735_write8(d, 0x02);
    st7735_write8(d, 0x07);
    st7735_write8(d, 0x09);
    st7735_write8(d, 0x0f);
    st7735_write8(d, 0x25);
    st7735_write8(d, 0x35);
    st7735_write8(d, 0x00);
    st7735_write8(d, 0x09);
    st7735_write8(d, 0x04);
    st7735_write8(d, 0x10);
    st7735_w_reg(d, 0xfc);//dot-inversed

    st7735_write8(d, 0x80);
    st7735_w_reg(d, 0x3a);
    st7735_write8(d, 0x05);
    st7735_w_reg(d, 0x36);
#if (USE_HORIZONTAL==0)
    st7735_write8(d, 0x08);
#elif (USE_HORIZONTAL==1)
    st7735_write8(d, 0xc8);
#elif (USE_HORIZONTAL==2)
    st7735_write8(d, 0x78);
#else
    st7735_write8(d, 0xa8);
#endif
    st7735_w_reg(d, 0x21);//display inversion
    st7735_w_reg(d, 0x29);//display on
    st7735_w_reg(d, 0x2a);//set column address
    st7735_write8(d, 0x00);
    st7735_write8(d, 0x1a);
    st7735_write8(d, 0x00);
    st7735_write8(d, 0x69);
    st7735_w_reg(d, 0x2b);//set page address
    st7735_write8(d, 0x00);
    st7735_write8(d, 0x01);
    st7735_write8(d, 0x00);
    st7735_write8(d, 0xa0);
    st7735_w_reg(d, 0x2c);
}

