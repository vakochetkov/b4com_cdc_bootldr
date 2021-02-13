/*
 * flash_stm32l0x2_drv.cpp
 *
 *  Created on: 13 янв. 2021 г.
 *      Author: kochetkov
 */

#include "flash_stm32l0x2_drv.hpp"

flash_stm32l0x2_drv_c<FLASH_BLOCK_SIZE,EEPROM_BLOCK_SIZE> flash_stm32l0x2_drv;

extern "C" {

__RAM_FUNC void __waitLastOperation() {
	while((FLASH->SR & FLASH_SR_BSY) != 0);

	if ((FLASH->SR & FLASH_SR_EOP) != 0) {
		FLASH->SR = FLASH_SR_EOP;
	} // else is ERROR
}

__RAM_FUNC void __writeFlashHalfPage(uint32_t addr, uint32_t * data) {
	__waitLastOperation();

	FLASH->PECR |= FLASH_PECR_FPRG | FLASH_PECR_PROG;

	__disable_irq();
	for (uint8_t i = 0; i < ((128 / 2) / sizeof(uint32_t)); i++) {
		*(__IO uint32_t *)(addr) = *data++;
	}
	__enable_irq();

	__waitLastOperation();
	FLASH->PECR &= ~(FLASH_PECR_FPRG | FLASH_PECR_PROG);
}

} // extern "C"


