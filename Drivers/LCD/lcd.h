/*
 * lcd.h
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "stm32f4xx_hal.h"

/* Constants */
#define DFT_SCAN_DIR  L2R_U2D
#define LCD_W 240
#define LCD_H 320

// Direction Adjustment
#define L2R_U2D  0x00
#define L2R_D2U  0x80
#define R2L_U2D  0x40
#define R2L_D2U  0xc0
#define U2D_L2R  0x20
#define U2D_R2L  0x60
#define D2U_L2R  0xa0
#define D2U_R2L  0xe0

// LCD Base Memory Address (FSMC)
#define LCD_BASE        ((uint32_t)(0x60000000 | 0x000ffffe))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

typedef struct {
	__IO uint16_t LCD_REG;
	__IO uint16_t LCD_RAM;
} LCD_TypeDef;


/* Core Functions for LVGL */
void lcd_init(void);
void lcd_set_address(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

#endif /* INC_LCD_H_ */