/*
 * rcc.hpp
 *
 *  Created on: 23 нояб. 2020 г.
 *      Author: kochetkov
 */

#ifndef INCLUDE_RCC_HPP_
#define INCLUDE_RCC_HPP_

#include "stm32l0xx_common.hpp"

extern "C" {
#define CORE_CLOCK 32000000
#define USB_CLOCK  48000000

extern volatile uint32_t SYS_TICK;
void SysTick_Handler(void);

void delay_ms(uint16_t ms);

} // extern "C"

class rcc_c {

public:
	/**
	 *	1. Set voltage regulator MODE (1 for performance)
	 *	2. Configure HSI and PLL, RC48 clocks and appropriate FLASH latency
	 *	3. Enable peripheral clocks
	 */
	static void InitClock(void) noexcept {
	    _BST(RCC->APB1ENR, RCC_APB1ENR_PWREN);
	    _BMD(PWR->CR, PWR_CR_VOS, PWR_CR_VOS_0);
	    _WBC(PWR->CSR, PWR_CSR_VOSF);
	    /* set FLASH latency to 1 */
	    _BST(FLASH->ACR, FLASH_ACR_LATENCY);
	    /* set clock at 32MHz PLL 6/3 HSI */
	    _BMD(RCC->CFGR, RCC_CFGR_PLLDIV | RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC, RCC_CFGR_PLLDIV3 | RCC_CFGR_PLLMUL6);
	    _BST(RCC->CR, RCC_CR_HSION);
	    _WBS(RCC->CR, RCC_CR_HSIRDY);
	    _BST(RCC->CR, RCC_CR_PLLON);
	    _WBS(RCC->CR, RCC_CR_PLLRDY);
	    /* switch clock to PLL */
	    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
	    _WVL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL);

	    _BST(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

	    __enable_irq();
	}

	static void InitSysTick() noexcept {
		SysTick_Config(CORE_CLOCK/1000);
	}

	static volatile uint32_t GetTick() noexcept {
		return SYS_TICK;
	}

	inline static void Reset(void) noexcept { NVIC_SystemReset(); }
};

typedef rcc_c rcc;


#endif /* INCLUDE_RCC_HPP_ */
