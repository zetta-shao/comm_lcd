#ifndef __ST7735_H
#define __ST7735_H		

#ifdef __cplusplus
extern "C" {
#endif
#include <stdlib.h>
#include "swgp.h"
#include "swspi.h"
#include "lcd_fonts.h"

#define USE_HORIZONTAL 2  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 80
#define LCD_H 160
#else
#define LCD_W 160
#define LCD_H 80
#endif

typedef struct st7735_t st7735_t;

struct st7735_t {
    //uint8_t     init;
    //uint8_t     unused[3];
    //uint16_t    colorf; //front color
    //uint16_t    colorb; //back color;
    lcddev_t    d;
    void        *dev;
    swgpio_t    *cs;
    swgpio_t    *dc; //data/command
    swgpio_t    *rst;
    swgpio_t    *blk;
    uint8_t     fbbuf[((LCD_W*LCD_H)/8)];
};

void st7735_gpioinit(st7735_t *d, swgpio_t *cs, swgpio_t *dc, swgpio_t *rst, swgpio_t *blk);
void st7735_writeN(st7735_t *d, uint8_t *val, int size);
void st7735_write8(st7735_t *d, uint8_t val);
void st7735_write16(st7735_t *d, uint16_t val);
void st7735_w_reg(st7735_t *d, uint8_t val);
void st7735_window(st7735_t *d, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void st7735_init(st7735_t *d, uint16_t width, uint16_t height, void *pvport, void *pvFontDef);

void st7735_fill(st7735_t *d, uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color);
void st7735_drawPoint(st7735_t *d, uint16_t x, uint16_t y, uint16_t color);
void st7735_drawLine(st7735_t *d, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void st7735_drawRectangle(st7735_t *d, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
void st7735_drawCircle(st7735_t *d, uint16_t x0,uint16_t y0,uint8_t r,uint16_t color);
void st7735_drawChar(st7735_t *d, char val, FontDef *pF);
void st7735_setcur(st7735_t *d, uint16_t x, uint16_t y);
void st7735_setcolor(st7735_t *d, uint16_t color_front, uint16_t color_back);

void st7735_pixeldraw(lcddev_t *d, uint16_t x, uint16_t y, uint8_t color);
void st7735_update_window(st7735_t *q, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color);//ָ�����������ɫ
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color);//��ָ��λ�û�һ����
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);//��ָ��λ�û�һ����
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);//��ָ��λ�û�һ������
void Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color);//��ָ��λ�û�һ��Բ

void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);//��ʾһ���ַ�
void LCD_ShowString(uint16_t x,uint16_t y,const uint8_t *p,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);//��ʾ�ַ���
void LCD_ShowIntNum(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey);//��ʾ��������
void LCD_ShowFloatNum1(uint16_t x,uint16_t y,float num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey);//��ʾ��λС������

void LCD_ShowPicture(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t pic[]);//��ʾͼƬ


//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //��ɫ
#define BRRED 			     0XFC07 //�غ�ɫ
#define GRAY  			     0X8430 //��ɫ
#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LGRAY 			     0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

extern const uint16_t st7735_colorpalette[];

#ifdef __cplusplus
}
#endif

#endif





