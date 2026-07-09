/*
 * touch.h
 */

#ifndef INC_TOUCH_H_
#define INC_TOUCH_H_

#include "main.h"

void touch_init(void);
void touch_Scan(void);
uint8_t touch_IsTouched(void);
uint16_t touch_GetX(void);
uint16_t touch_GetY(void);

#endif /* INC_TOUCH_H_ */