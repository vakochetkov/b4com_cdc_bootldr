/*
 * flash.hpp
 *
 *  Created on: 13 дек. 2020 г.
 *      Author: kochetkov
 */

#ifndef INCLUDE_FLASH_HPP_
#define INCLUDE_FLASH_HPP_

#include "flash_stm32l0x2_drv.hpp"
#include "crc32.hpp"

constexpr uint32_t CHUNK_SIZE = 128;
constexpr uint32_t MAGICWORD_ADDR_OFFSET = 0x0;

constexpr uint32_t FIRMWARE_MAX_SIZE   = 0x8000;
constexpr uint32_t FIRMWARE_ADDR_START = FLASH_BASE_ADDR + 0x8000;


template<auto TDriver, size_t TWriteBlockSize>
class flash_c {

public:
	static void Init() noexcept {
		TDriver->Init();
		TDriver->UnlockEEPROM();
		TDriver->UnlockNVM();
	}

	static inline size_t GetBlockSize() {
		return TWriteBlockSize;
	}

	static void WriteChunk(uint32_t chunkNumber, uint32_t * data, uint32_t length) noexcept {
		if (length != TWriteBlockSize) {
			return;
		}
		TDriver->UnlockEEPROM();
		TDriver->UnlockNVM();
		TDriver->WriteBlockNVM(FIRMWARE_ADDR_START + (chunkNumber * 128), data, 128 / sizeof(uint32_t));
	}

	static void ReadChunk(uint32_t chunkNumber, uint32_t * data, uint32_t length) noexcept {
		if (length < TWriteBlockSize) {
			return;
		}
		TDriver->ReadBlockNVM(FIRMWARE_ADDR_START + (chunkNumber * 128), data, 128 / sizeof(uint32_t));
	}

	static void EraseAll() noexcept {
		TDriver->UnlockEEPROM();
		TDriver->UnlockNVM();
		for (uint32_t addr = FIRMWARE_ADDR_START; addr < (FIRMWARE_ADDR_START + FIRMWARE_MAX_SIZE); addr += 128) {
			TDriver->EraseBlockNVM(addr);
		}
	}

	// relative addr, not absolute
	static inline void WriteWordEEPROM(uint32_t word) noexcept {
		TDriver->UnlockEEPROM();
		TDriver->WriteBlockEEPROM(EEPROM_BASE_ADDR + MAGICWORD_ADDR_OFFSET, &word, 1);
	}

	static inline uint32_t ReadWordEEPROM() noexcept {
		uint32_t tmp = 0;
		TDriver->ReadBlockEEPROM(EEPROM_BASE_ADDR + MAGICWORD_ADDR_OFFSET, &tmp, 1);
		return tmp;
	}
};

typedef flash_c<&flash_stm32l0x2_drv,CHUNK_SIZE> flash; // for STM32L052 should be 64 byte (half-page) aligned

#endif /* INCLUDE_FLASH_HPP_ */
