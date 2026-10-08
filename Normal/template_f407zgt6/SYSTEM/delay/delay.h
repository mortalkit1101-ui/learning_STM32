#ifndef TEMPLATE_DELAY_H
#define TEMPLATE_DELAY_H

#include "stm32f4xx.h"

/* Owns SysTick; call again after changing the system clock. */
void delay_init(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

#endif
