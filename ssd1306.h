/**
 * This Library was originally written by Olivier Van den Eede (4ilo) in 2016.
 * Some refactoring was done and SPI support was added by Aleksander Alekseev (afiskon) in 2018.
 *
 * https://github.com/afiskon/stm32-ssd1306
 */

#ifndef __SSD1306_H__
#define __SSD1306_H__
#ifdef __cplusplus
extern "C" {
#endif

//#include <stddef.h>
#include <stdint.h>
//#include <_ansi.h>
#include <swgp.h>
#include "ssd1306_conf.h"

#if 0
#if defined(STM32F0)
#include "stm32f0xx_hal.h"
#elif defined(STM32F1)
#include "stm32f1xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#elif defined(STM32L0)
#include "stm32l0xx_hal.h"
#elif defined(STM32L1)
#include "stm32l1xx_hal.h"
#elif defined(STM32L4)
#include "stm32l4xx_hal.h"
#elif defined(STM32L5)
#include "stm32l5xx_hal.h"
#elif defined(STM32F3)
#include "stm32f3xx_hal.h"
#elif defined(STM32H7)
#include "stm32h7xx_hal.h"
#elif defined(STM32F7)
#include "stm32f7xx_hal.h"
#elif defined(STM32G0)
#include "stm32g0xx_hal.h"
#elif defined(STM32G4)
#include "stm32g4xx_hal.h"
//#else
//#error "SSD1306 library was tested only on STM32F0, STM32F1, STM32F3, STM32F4, STM32F7, STM32L0, STM32L1, STM32L4, STM32H7, STM32G0, STM32G4 MCU families. Please modify ssd1306.h if you know what you are doing. Also please send a pull request if it turns out the library works on other MCU's as well!"
#endif
#endif

#ifdef SSD1306_X_OFFSET
#define SSD1306_X_OFFSET_LOWER (SSD1306_X_OFFSET & 0x0F)
#define SSD1306_X_OFFSET_UPPER ((SSD1306_X_OFFSET >> 4) & 0x07)
#else
#define SSD1306_X_OFFSET_LOWER 0
#define SSD1306_X_OFFSET_UPPER 0
#endif

#define SPI_3PIN 1

#include "lcd_fonts.h"

/* vvv I2C config vvv */

#ifndef SSD1306_I2C_PORT
#define SSD1306_I2C_PORT        hi2c1
#endif

#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR        (0x3C << 1)
#endif

/* ^^^ I2C config ^^^ */
/* vvv SPI config vvv */
/* ^^^ SPI config ^^^ */

#include <swgp.h>
#include <swi2c.h>
#include <swspi.h>

// SSD1306 OLED height in pixels
//#ifndef SSD1306_HEIGHT
//#define SSD1306_HEIGHT          64
//#endif

// SSD1306 width in pixels
//#ifndef SSD1306_WIDTH
//#define SSD1306_WIDTH           128
//#endif

//#ifndef SSD1306_BUFFER_SIZE
//#define SSD1306_BUFFER_SIZE   SSD1306_WIDTH * SSD1306_HEIGHT / 8
//#endif

//#define SSD1306_BUFFER_SIZE (132*8)

typedef swgpio_t ssd1306_gpio_t; //change for other MCU
//typedef amd64_gpio_t ssd1306_gpio_t; //change for other MCU

// Enumeration for screen colors
typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} SSD1306_COLOR;

typedef enum {
    SSD1306_OK = 0x00,
    SSD1306_ERR = 0x01  // Generic error.
} SSD1306_Error_t;

typedef struct SSD1306_t SSD1306_t;
typedef struct SSD1306_VERTEX SSD1306_VERTEX;

// Struct to store transformations
struct SSD1306_t {
	lcddev_t	d;
	uint8_t		hwflag;
    uint8_t		i2caddr;
    swgpio_t	*DC;
    swgpio_t	*CS;
    swgpio_t	*RST;
    void        *pDev;
    //uint8_t		SSD1306_Buffer[SSD1306_BUFFER_SIZE];
    uint8_t		*SSD1306_Buffer;
};

#define ssd1306_t SSD1306_t

struct SSD1306_VERTEX {
    uint8_t x;
    uint8_t y;
};

#define __INITED		0x01
#define __DISPLAY_ON	0x02
#define __OLED_I2C      0x40    //using i2c
#define __OLED_3WSPI	0x80    //9bit SPI

/*
 * usage:
 *   SPI: 5/4/3 wired use SSD1306_gpioinit5W2/SSD1306_gpioinit4W2/SSD1306_gpioinit3W2, then SSD1306_init
 *   I2C: SSD1306_I2Cinit, then SSD1306_init.
 *  pvport: SPI/I2C device header, pvfontDef: font, from lcd_fonts.h
 */
void SSD1306_gpioinit5W2(SSD1306_t *d, swgpio_t *CS, swgpio_t *DC, swgpio_t *RST);
void SSD1306_gpioinit4W2(SSD1306_t *d, swgpio_t *CS, swgpio_t *DC);
void SSD1306_gpioinit3W2(SSD1306_t *d, swgpio_t *CS);
void SSD1306_I2Cinit(SSD1306_t *d, uint8_t addr);
void SSD1306_init(SSD1306_t *d, uint8_t width, uint8_t height, void *pvport, void *pvFontDef);
void SH1106_Init(SSD1306_t *d, void *pvport);
void ssd1306_fill(lcddev_t *d, uint8_t color);
void ssd1306_update(lcddev_t *p);

void ssd1306_pixeldraw(fontdraw_t *d, uint16_t x, uint16_t y, uint8_t color);

/**
 * @brief Sets the contrast of the display.
 * @param[in] value contrast to set.
 * @note Contrast increases as the value increases.
 * @note RESET = 7Fh.
 */
void ssd1306_SetContrast(SSD1306_t *d, const uint8_t value);

/**
 * @brief Set Display ON/OFF.
 * @param[in] on 0 for OFF, any for ON.
 */
void ssd1306_SetDisplayOn(SSD1306_t *d, const uint8_t on);

/**
 * @brief Reads DisplayOn state.
 * @return  0: OFF.
 *          1: ON.
 */
uint8_t ssd1306_GetDisplayOn();

// Low-level procedures
void ssd1306_Reset(SSD1306_t *d);
void ssd1306_WriteCommand(SSD1306_t *d, uint8_t byte);
void ssd1306_WriteData(SSD1306_t *d, uint8_t* buffer, uint32_t buff_size);
SSD1306_Error_t ssd1306_FillBuffer(SSD1306_t *d, uint8_t* buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif // __SSD1306_H__
