/*
 * crc32.hpp
 * CRC32 Ethernet/MPEG-2 HW block driver
 * TODO: DMA burst transfers
 *
 *  Created on: 12 февр. 2021 г.
 *      Author: kochetkov
 */

#ifndef INCLUDE_CRC32_HPP_
#define INCLUDE_CRC32_HPP_

#include "stm32l0xx_common.hpp"


template<uint32_t TInitValue, uint32_t TPolyValue>
class crc32_c {

public:
	static void Init() noexcept {
		RCC->AHBENR |= RCC_AHBENR_CRCEN;

		CRC->INIT = TInitValue;
		CRC->CR = 0x0; 				// none-reverse, 32-bit
		CRC->POL = TPolyValue;
		CRC->CR |= CRC_CR_RESET;	// automatically cleared
	}

	static void DeInit() noexcept {
		CRC->CR |= CRC_CR_RESET;
		RCC->AHBENR &= ~RCC_AHBENR_CRCEN;
	}

	static uint32_t CalcWordBlocking(uint32_t word) noexcept {
		CRC->DR = word;
		return (CRC->DR);
	}

	static uint32_t CalcBufferBlocking(uint32_t * buf, uint32_t length) noexcept {
		if ((length == 0) || (buf == nullptr)) {
			return 0;
		}
		if (length == 1) {
			return CalcWordBlocking(buf[0]);
		}

		for (uint32_t i = 0; i < length; i++) {
			CRC->DR = buf[i];
		}

		return (CRC->DR);
	}
};

typedef crc32_c<0xFFFFFFFF, 0x04C11DB7> crc32;


#endif /* INCLUDE_CRC32_HPP_ */
