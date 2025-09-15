#ifndef __LCD1602SW__
#define __LCD1602SW__
#ifdef __cplusplus
extern "C" {
#endif

#define LCD_ADDRESS 0x27
//#define LCD_ADDRESS_8 (LCD_ADDRESS << 1)
#define LCD_BACKLIGHT	0x8

typedef struct lcd1602_t lcd1602_t;
//typedef swi2c_t;

typedef union { //0x01
	struct {
	uint8_t data_l : 4;
	uint8_t data_h : 4;
	} __attribute__((packed));
	uint8_t i2cpkg;
} i2cpkg;

struct lcd1602_t {
	swi2c_t 	*d;
	uint8_t 	i2c_addr;
	uint8_t		bklg;
	uint8_t		log[2];
};

void lcd1602_send_string (lcd1602_t *p, char *str);
int8_t lcd1602_init(lcd1602_t *p, swi2c_t *d, uint8_t i2c_addr);
void lcd1602_clear(lcd1602_t *p);
void lcd1602_put_cur(lcd1602_t *p, uint8_t row,uint8_t col);
void lcd1602_set_backlight_on(lcd1602_t *p, uint8_t on);

#ifdef __cplusplus
}
#endif

#endif
