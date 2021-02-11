/*
 * bootloader.hpp
 *
 *  Created on: 13 дек. 2020 г.
 *      Author: kochetkov
 */

#ifndef INCLUDE_BOOTLOADER_HPP_
#define INCLUDE_BOOTLOADER_HPP_

#include "stm32l0xx_common.hpp"
#include "rcc.hpp"
#include "cdc.hpp"
#include "led.hpp"
#include "flash.hpp"
#include "timeout.hpp"

extern "C" {
#include "retarget_bkpt.h"
}

enum class btldr_magic_word_t : uint32_t {
	MW_FLASH_EMPTY       = 0xFFFFFFFF,
	MW_FLASH_ZERO        = 0x00000000,
	MW_FLASH_IS_UPDATED  = 0xB4C0CCCC,
	MW_FLASH_NEED_UPDATE = 0xB4C0AAAA
};

typedef union {
	struct fields {
		char s;
		uint32_t size;
		char a;
		uint32_t addr;
		char c;
		uint32_t crc;
		char b;
		uint32_t chunk;
	} __attribute__((__packed__));

	uint8_t data[1 + 4 + 1 + 4 + 1 + 4 + 1 + 4];
} btldr_params_t;

typedef union {
	struct fields {
		char b;
		uint32_t block_number;
		char d;
		uint8_t data_block[CHUNK_SIZE];
	} __attribute__((__packed__));

	uint8_t data[1 + 4 + 1 + CHUNK_SIZE];
} btldr_chunk_t;


template<size_t TBlockSize> // FW block size
class bootloader_c {
	enum class btldr_msg_t : uint8_t {
		MSG_START   = 0,
		MSG_DEVNAME,
		MSG_CHECK,
	};
	enum class btldr_respond_t : uint8_t {
		RSPD_ACK       = 0, // common "OK" respond
		RSPD_BAD_DEVNAME,   // invalid device name (hard-coded)
		// PARAMS_READ stage
		RSPD_BAD_PARAMS,
		RSPD_BAD_SIZE,
		RSPD_BAD_ADDR,
		RSPD_BAD_CRC,
		RSPD_BAD_CHUNK,
		RSPD_BAD_CHSZ_RATIO,
		// FLASH stage
		RSPD_BAD_FW_CHUNK,
		RSPD_BAD_FW_CHUNKNUM,
		// CHECK stage
		RSPD_BAD_FLASH_CMD,
		RSPD_BAD_FLASH_READ,
		RSPD_BAD_FLASH_CRC,
	};
	static const char * btldr_cmd_msg[];
	static const char * btldr_respond_msg[];

	static btldr_params_t params;
	static btldr_chunk_t  fwblock;

public:
	static void Init() noexcept {
		flash::Init();
		crc32::Init();

	}

	static void SelfTest() noexcept {
		uint32_t word = flash::ReadWordEEPROM();
		SHTRACE("#1 Read MW: %#010x", word);

		flash::WriteWordEEPROM(static_cast<uint32_t>(btldr_magic_word_t::MW_FLASH_NEED_UPDATE));
		word = flash::ReadWordEEPROM();
		SHTRACE("#2 Read MW: %#010x", word);

//		flash::EraseAll(); // TODO: flash write/erase/read
	}
};

template<size_t TBlockSize>
const char * bootloader_c<TBlockSize>::btldr_cmd_msg[] = {
    "BTLDR_START",
	"BTLDR_B4COM_OUTLET_V2",
	"BTLDR_CHECK"
};

template<size_t TBlockSize>
const char * bootloader_c<TBlockSize>::btldr_respond_msg[] = {
    "BTLDR_ACK\n",
	"BTLDR_BAD_DEVNAME\n",
	"BTLDR_BAD_PARAMS\n",
	"BTLDR_BAD_SIZE\n",
	"BTLDR_BAD_ADDR\n",
	"BTLDR_BAD_CRC\n",
	"BTLDR_BAD_CHUNK\n",
	"BTLDR_BAD_CHSZ_RATIO\n",
	"BTLDR_BAD_FW_CHUNK\n",
	"BTLDR_BAD_BAD_FW_CHUNKNUM\n",
	"BTLDR_BAD_FLASH_CMD\n",
	"BTLDR_BAD_FLASH_READ\n",
	"BTLDR_BAD_FLASH_CRC\n"
};

template<size_t TBlockSize>
btldr_params_t bootloader_c<TBlockSize>::params = {0};

template<size_t TBlockSize>
btldr_chunk_t bootloader_c<TBlockSize>::fwblock = {0};

typedef bootloader_c<CHUNK_SIZE> bootloader;


#endif /* INCLUDE_BOOTLOADER_HPP_ */
