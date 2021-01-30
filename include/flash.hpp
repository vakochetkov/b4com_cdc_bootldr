/*
 * flash.hpp
 *
 *  Created on: 13 дек. 2020 г.
 *      Author: kochetkov
 */

#ifndef INCLUDE_FLASH_HPP_
#define INCLUDE_FLASH_HPP_

#include "flash_stm32l0x2_drv.hpp"

template<auto TDriver, size_t TWriteBlockSize>
class flash_c {

public:
	static void Init() noexcept {
		TDriver->Init();
	}

	static void Write() noexcept {

	}

	static void Read() noexcept {

	}

	static void Erase() noexcept {

	}
};

typedef flash_c<&flash_stm32l0x2_drv,128> flash; // for STM32L052 should be 64 byte (half-page) aligned

#endif /* INCLUDE_FLASH_HPP_ */
