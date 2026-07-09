/*
 * touch.c
 */

#include "touch.h"
#include "at24c.h" // Kept for loading your saved calibration
#include "lcd.h"
#include <stdlib.h>

// Calibration factors (loaded from EEPROM)
static float xfac = 0;
static float yfac = 0;
static short xoff = 0;
static short yoff = 0;

// Current coordinates
static uint16_t current_x = 0;
static uint16_t current_y = 0;

#define SAVE_ADDR_BASE 0

#if (DFT_SCAN_DIR>>4)%4
uint8_t CMD_RDX=0X90;
uint8_t CMD_RDY=0XD0;
#else
uint8_t CMD_RDX=0XD0;
uint8_t CMD_RDY=0X90;
#endif

static void TP_Write_Byte(uint8_t num) {
	uint8_t count=0;
	for(count=0;count<8;count++) {
		if(num&0x80) HAL_GPIO_WritePin(T_MOSI_GPIO_Port, T_MOSI_Pin, 1);
		else HAL_GPIO_WritePin(T_MOSI_GPIO_Port, T_MOSI_Pin, 0);
		num<<=1;
		HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 0);
		// delay_us(1); // Usually not strictly needed for F4, but add back if your SPI is too fast
		HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 1);
	}
}

static uint16_t TP_Read_AD(uint8_t CMD) {
	uint8_t count=0;
	uint16_t Num=0;
	HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 0);
	HAL_GPIO_WritePin(T_MOSI_GPIO_Port, T_MOSI_Pin, 0);
	HAL_GPIO_WritePin(T_CS_GPIO_Port, T_CS_Pin, 0);
	TP_Write_Byte(CMD);
	
	HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 0);
	HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 1);
	HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 0);
	
	for(count=0;count<16;count++) {
		Num<<=1;
		HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 0);
		HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, 1);
 		if(HAL_GPIO_ReadPin(T_MISO_GPIO_Port, T_MISO_Pin) != 0) Num++;
	}
	Num>>=4;
	HAL_GPIO_WritePin(T_CS_GPIO_Port, T_CS_Pin, 1);
	return(Num);
}

#define READ_TIMES 5
#define LOST_VAL 1
static uint16_t TP_Read_XOY(uint8_t xy) {
	uint16_t i, j, temp, buf[READ_TIMES], sum=0;
	for(i=0;i<READ_TIMES;i++) buf[i]=TP_Read_AD(xy);
	for(i=0;i<READ_TIMES-1; i++) {
		for(j=i+1;j<READ_TIMES;j++) {
			if(buf[i]>buf[j]) {
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++) sum+=buf[i];
	return sum/(READ_TIMES-2*LOST_VAL);
}

static uint8_t TP_Read_XY(uint16_t *x,uint16_t *y) {
	*x = TP_Read_XOY(CMD_RDX);
	*y = TP_Read_XOY(CMD_RDY);
	return 1;
}

#define ERR_RANGE 100
static uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y) {
	uint16_t x1,y1, x2,y2;
	if(!TP_Read_XY(&x1,&y1)) return 0;
	if(!TP_Read_XY(&x2,&y2)) return 0;
	
	if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))
	&&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE))) {
		*x=(x1+x2)/2;
		*y=(y1+y2)/2;
		return 1;
	}
	return 0;
}

static uint8_t TP_Get_Adjdata(void) {
	// Read orientation flag
	uint8_t temp = at24c_ReadOneByte(SAVE_ADDR_BASE+14);
	if(temp == DFT_SCAN_DIR) {
        // Read calibration data from EEPROM
		at24c_Read(SAVE_ADDR_BASE,(uint8_t*)&xfac,14);
		return 1;
	}
	// Defaults if EEPROM is empty or orientation changed
    xfac = 0.08; // Example fallback values
    yfac = 0.06;
    xoff = -15;
    yoff = -20;
	return 0;
}

void touch_init(void) {
	at24c_init();
	TP_Get_Adjdata(); // Load your saved calibration factors
}

void touch_Scan(void) {
	uint16_t raw_x, raw_y;
	if(HAL_GPIO_ReadPin(T_PEN_GPIO_Port, T_PEN_Pin) == GPIO_PIN_RESET) {
		if(TP_Read_XY2(&raw_x, &raw_y)) {
			// Apply calibration math to get real screen pixels
	 		current_x = (uint16_t)(xfac * raw_x + xoff);
			current_y = (uint16_t)(yfac * raw_y + yoff);
	 	}
	}
}

uint8_t touch_IsTouched(void) {
	return (HAL_GPIO_ReadPin(T_PEN_GPIO_Port, T_PEN_Pin) == GPIO_PIN_RESET);
}

uint16_t touch_GetX(void) { return current_x; }
uint16_t touch_GetY(void) { return current_y; }