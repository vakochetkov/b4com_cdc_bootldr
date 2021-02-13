/*
 * bootloader.cpp
 *
 *  Created on: 13 дек. 2020 г.
 *      Author: kochetkov
 */

#include "bootloader.hpp"

extern "C" void JumpToApp(uint32_t addr) {
	void (*Jump)(void);

	uint32_t jumpAddr = *((volatile uint32_t*)(addr + 4)); // set to Reset_Handler
	Jump = (void (*)(void))jumpAddr;

	__disable_irq();
	SCB->VTOR = addr;
	__set_MSP(*((volatile uint32_t*) addr)); // set stack pointer
//	Jump();
	NVIC_SystemReset(); // alternative
}



