/*
 * bootloader.hpp
 *
 *  Created on: 13 дек. 2020 г.
 *      Author: kochetkov
 */

#ifndef INCLUDE_BOOTLOADER_HPP_
#define INCLUDE_BOOTLOADER_HPP_

#include <cstring>
#include "stm32l0xx_common.hpp"
#include "rcc.hpp"
#include "cdc.hpp"
#include "led.hpp"
#include "flash.hpp"
#include "timeout.hpp"

extern "C" {
#include "retarget_bkpt.h"
}

extern "C" void JumpToApp(uint32_t addr);

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
		char n;
		uint32_t num;
	} __attribute__((__packed__));

	uint8_t data[1 + 4 + 1 + 4 + 1 + 4 + 1 + 4 + 1 + 4];
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
	enum class btldr_state_t : uint8_t {
		ST_IDLE = 0,
		ST_START,
		ST_DEVNAME,
		ST_PARAMS,
		ST_FLASH,
		ST_CHECK,
		ST_FINISH
	};
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
		RSPD_BAD_CHNUM,
		// FLASH stage
		RSPD_BAD_FW_CHUNK,
		RSPD_BAD_FW_CHUNKNUM,
		// CHECK stage
		RSPD_BAD_FLASH_CMD,
		RSPD_BAD_FLASH_READ,
		RSPD_BAD_FLASH_CRC,
	};
	static const char PREFIX[];
	static const char * btldr_cmd_msg[];
	static const char * btldr_respond_msg[];

	static btldr_params_t params;
	static btldr_chunk_t  fwblock;

	static inline void DeInitAndReset() noexcept {
//		cdc::DeInit(); // xXX: debug
		rcc::Reset();
	}

	static bool parseParams(char * buf, uint32_t length) noexcept {
		return 1;
	}

public:
	static void Init() noexcept {
		flash::Init();
		crc32::Init();

		uint32_t word = flash::ReadWordEEPROM();
		if (word == static_cast<uint32_t>(btldr_magic_word_t::MW_FLASH_IS_UPDATED)) {
			SHTRACE("FW is up to date, jump...");
			JumpToApp(FIRMWARE_ADDR_START);
		} else {
			// need update
		}
	}

	static void ProcessNext(char * buf, uint32_t length) noexcept {
		static char * chptr = NULL;
		static btldr_state_t state = btldr_state_t::ST_IDLE;
		static Timeout t;


		switch(state) {
		case btldr_state_t::ST_IDLE:
			t.Set(30000);
			state = btldr_state_t::ST_START;
			break;

		case btldr_state_t::ST_START:
			led::Set(1,1);
			chptr = strstr(buf, btldr_cmd_msg[static_cast<uint8_t>(btldr_msg_t::MSG_START)]);
			if (t.IsTimeOut()) {
				SHTRACE("START timeout");
				DeInitAndReset();
			}
			if (chptr != NULL) {
				SHTRACE("START detected");
				cdc::WriteBlk(btldr_respond_msg[static_cast<uint8_t>(btldr_respond_t::RSPD_ACK)],
								strlen(btldr_respond_msg[static_cast<uint8_t>(btldr_respond_t::RSPD_ACK)]));
				state = btldr_state_t::ST_DEVNAME;
				t.Clear();
				t.Set(2000);
			} else {
				t.Update();
			}
			break;

		case btldr_state_t::ST_DEVNAME:
			led::Set(2,1);
			chptr = strstr(buf, btldr_cmd_msg[static_cast<uint8_t>(btldr_msg_t::MSG_DEVNAME)]);
			if (t.IsTimeOut()) {
				SHTRACE("DEVNAME timeout");
				DeInitAndReset();
			}
			if (chptr != NULL) {
				SHTRACE("DEVNAME detected");
				cdc::WriteBlk(btldr_respond_msg[static_cast<uint8_t>(btldr_respond_t::RSPD_ACK)],
								strlen(btldr_respond_msg[static_cast<uint8_t>(btldr_respond_t::RSPD_ACK)]));
				state = btldr_state_t::ST_PARAMS;
				t.Clear();
				t.Set(2000);
			} else {
				t.Update();
			}
			break;

		case btldr_state_t::ST_PARAMS:
			led::Set(3,1);
//			chptr = strstr(buf, PREFIX);

			if (t.IsTimeOut()) {
				SHTRACE("PARAMS timeout");
				DeInitAndReset();
			}

			if (parseParams(buf, length)) {
				SHTRACE("PARAMS parsed");
				state = btldr_state_t::ST_FLASH;
				t.Clear();
				t.Set(2000);
			} else {
				t.Update();
			}
			if (chptr != NULL) {
				auto remainLen = length - (chptr - buf) - strlen(PREFIX);
				SHTRACE("PARAMS detected");
				SHTRACE("%s", chptr + strlen(PREFIX));
				SHTRACE("remain: %lu", remainLen);
				if (parseParams(chptr + strlen(PREFIX), remainLen)) {
					SHTRACE("test");
					__BKPT(2);
				}
			} else {
				t.Update();
			}
			break;

		case btldr_state_t::ST_FLASH:
			SHTRACE("ST_FLASH");
			__BKPT(2);
			break;

		default: DeInitAndReset();
		}

	}

	static void SelfTest() noexcept {
		static uint32_t data[32] = {0};

		uint32_t word = flash::ReadWordEEPROM();
		SHTRACE("#1 Read MW: %#010x", word);

		flash::WriteWordEEPROM(static_cast<uint32_t>(btldr_magic_word_t::MW_FLASH_NEED_UPDATE));
		word = flash::ReadWordEEPROM();
		SHTRACE("#2 Read MW: %#010x", word);

		SHTRACE("\r\n %#010x %#010x %#010x \r\n", data[0], data[1], data[2]);
		flash::ReadChunk(0, &data[0], 128);
		SHTRACE("\r\n %#010x %#010x %#010x \r\n", data[0], data[1], data[2]);

		flash_stm32l0x2_drv.EraseBlockNVM(0x800FF80);
		flash::EraseChunk(0);
		flash::ReadChunk(0, &data[0], 128);
		SHTRACE("\r\n %#010x %#010x %#010x \r\n", data[0], data[1], data[2]);

		data[0] = 0x000000AA;
		data[2] = 0x0000ABCD;
		SHTRACE("\r\n %#010x %#010x %#010x \r\n", data[0], data[1], data[2]);
		flash::WriteChunk(0, &data[0], 128);
//		flash_stm32l0x2_drv.WriteBlockNVM(0x08008000, data, 128);


		flash::ReadChunk(0, &data[0], 128);
		SHTRACE("\r\n %#010x %#010x %#010x \r\n", data[0], data[1], data[2]);

		uint32_t crc = crc32::CalcBufferBlocking(data, 3);
		SHTRACE("\r\n %#010x \r\n", crc);

		SHTRACE("Self-Test finished!");
		while(1);
	}
};

template<size_t TBlockSize>
const char bootloader_c<TBlockSize>::PREFIX[] = "BTLDR_";

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
	"BTLDR_BAD_CHNUM\n",
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
