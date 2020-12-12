/*
 * rcc.cpp
 *
 *  Created on: 10 дек. 2020 г.
 *      Author: kochetkov
 */

#include "rcc.hpp"

extern "C" {
volatile uint32_t SYS_TICK = 0;

void SysTick_Handler(void) {
	SYS_TICK++;
}

/**
 * Blocking delay on SysTick
 * @param ms - delay in ms
 */
void delay_ms(uint16_t ms) {
	static uint32_t start = 0;

	start = SYS_TICK;
	while((SYS_TICK - start) < ms);
}

} // extern "C"




