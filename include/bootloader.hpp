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
	struct {
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
	} __attribute__((__packed__)) fields;

	uint8_t data[1 + 4 + 1 + 4 + 1 + 4 + 1 + 4 + 1 + 4];
} btldr_params_t;

typedef union { // WARN! data_block location MUST be 32bit aligned!
	struct {
		uint32_t bid;
		uint32_t block_number;
		uint32_t did;
		uint32_t data_block[CHUNK_SIZE / sizeof(uint32_t)];
	} __attribute__((__packed__)) fields;

	uint8_t data[4 + 4 + 4 + CHUNK_SIZE];
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
	enum class parser_state_t : uint8_t {
		ST_IDLE = 0,
		ST_HOOKED,
		ST_COMPLETE
	};
	enum class flasher_state_t : uint8_t {
		ST_IDLE = 0,
		ST_HOOKED,
		ST_COMPLETE
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
//		RSPD_BAD_CHSZ_RATIO,
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

	static inline void disconnectAndReset() noexcept {
		led::SetAll(0);
		delay_ms(100);
//		cdc::DeInit(); // xXX: debug
		rcc::Reset();
	}

	static void __printParams() noexcept {
		SHTRACE("%c %lu", params.fields.s, params.fields.size);
		SHTRACE("%c %#010x", params.fields.a, params.fields.addr);
		SHTRACE("%c %#010x", params.fields.c, params.fields.crc);
		SHTRACE("%c %lu", params.fields.b, params.fields.chunk);
		SHTRACE("%c %lu", params.fields.n, params.fields.num);
	}

	static inline uint32_t calcChunkNumber() noexcept {
		return ((params.fields.size / params.fields.chunk) + (params.fields.size % params.fields.chunk > 0));
	}

	// current limitations: fixed start addr, fixed chunk size
	static void validateParams() noexcept {
		auto result = btldr_respond_t::RSPD_ACK;

		// errors fallthrough, firstly send last error
		if (params.fields.s != 'S') {
			SHTRACE("S invalid: %c", params.fields.s);
			result = btldr_respond_t::RSPD_BAD_PARAMS;
		}
		if (params.fields.size > FIRMWARE_MAX_SIZE) {
			SHTRACE("SIZE invalid: %lu", params.fields.size);
			result = btldr_respond_t::RSPD_BAD_SIZE;
		}
		if (params.fields.a != 'A') {
			SHTRACE("A invalid: %c", params.fields.a);
			result = btldr_respond_t::RSPD_BAD_PARAMS;
		}
		if (params.fields.addr != FIRMWARE_ADDR_START) {
			SHTRACE("ADDR invalid: %#010x", params.fields.a);
			result = btldr_respond_t::RSPD_BAD_ADDR;
		}
		if (params.fields.c != 'C') {
			SHTRACE("C invalid: %c", params.fields.c);
			result = btldr_respond_t::RSPD_BAD_PARAMS;
		}
		if (params.fields.crc == 0) {
			SHTRACE("CRC invalid: %#010x", params.fields.crc);
			result = btldr_respond_t::RSPD_BAD_CRC;
		}
		if (params.fields.b != 'B') {
			SHTRACE("B invalid: %c", params.fields.b);
			result = btldr_respond_t::RSPD_BAD_PARAMS;
		}
		if (params.fields.chunk != TBlockSize) {
			SHTRACE("CHUNK invalid: %lu", params.fields.chunk);
			result = btldr_respond_t::RSPD_BAD_CHUNK;
		}
		if (params.fields.n != 'N') {
			SHTRACE("N invalid: %c", params.fields.n);
			result = btldr_respond_t::RSPD_BAD_PARAMS;
		}
		if (params.fields.num != calcChunkNumber()) {
			SHTRACE("NUM invalid: %lu vs %lu", params.fields.num, calcChunkNumber());
			result = btldr_respond_t::RSPD_BAD_CHNUM;
		}

		cdc::WriteBlk(btldr_respond_msg[static_cast<uint8_t>(result)],
						strlen(btldr_respond_msg[static_cast<uint8_t>(result)]));
		if (result != btldr_respond_t::RSPD_ACK) {
			disconnectAndReset();
		}
	}

	static void validateChunk() noexcept {
		auto result = btldr_respond_t::RSPD_ACK;

		if (fwblock.fields.bid != 0xBBBB) {
			SHTRACE("B invalid: %c", fwblock.fields.bid);
			result = btldr_respond_t::RSPD_BAD_FW_CHUNK;
		}
		if (fwblock.fields.block_number >= params.fields.num) {
			SHTRACE("BNUM invalid: %c", fwblock.fields.block_number);
			result = btldr_respond_t::RSPD_BAD_FW_CHUNKNUM;
		}
		if (fwblock.fields.did != 0xDDDD) {
			SHTRACE("D invalid: %c", fwblock.fields.did);
			result = btldr_respond_t::RSPD_BAD_FW_CHUNK;
		}

		if (result != btldr_respond_t::RSPD_ACK) {
			cdc::WriteBlk(btldr_respond_msg[static_cast<uint8_t>(result)],
							strlen(btldr_respond_msg[static_cast<uint8_t>(result)]));
			disconnectAndReset();
		}
	}

	static void flashChunk() noexcept {

		flash::EraseChunk(fwblock.fields.block_number);
		flash::WriteChunk(fwblock.fields.block_number, &fwblock.fields.data_block[0], TBlockSize);
	}

	static bool parseParams(char * buf, uint32_t length) noexcept {
		static parser_state_t state = parser_state_t::ST_IDLE;
		static uint32_t remain = 0;
		bool result = false;
		char * chptr = NULL;

		switch(state) {
		case parser_state_t::ST_IDLE:
			chptr = strstr(buf, PREFIX);

			if (chptr != NULL) {
				remain = length - (chptr - buf) - strlen(PREFIX);

				if (remain < sizeof(params)) {
					SHTRACE("params fragmented!");
					memcpy(&params.data[0], (chptr + strlen(PREFIX)), remain);
					remain = sizeof(params) - remain;
					state = parser_state_t::ST_HOOKED;
				} else {
					SHTRACE("params received");
					memcpy(&params.data[0], (chptr + strlen(PREFIX)), sizeof(params));
					state = parser_state_t::ST_COMPLETE;
				}
			}
			break;

		case parser_state_t::ST_HOOKED: // append params if message was fragmented
			if (remain <= length) {
				memcpy(&params.data[sizeof(params) - remain], &buf[0], remain);
				state = parser_state_t::ST_COMPLETE;
			} else {
				memcpy(&params.data[sizeof(params) - remain], &buf[0], length);
				remain = remain - length;
			}
			break;

		case parser_state_t::ST_COMPLETE:
//			__printParams();
			validateParams();
			result = true;
			break;

		default: SHTRACE("params parser error!"); disconnectAndReset();
		}
		return result;
	}

	static bool parseChunk(char * buf, uint32_t length) noexcept {
		static flasher_state_t state = flasher_state_t::ST_IDLE;
		static uint32_t remain = 0;
		bool result = false;
		char * chptr = NULL;

		switch(state) {
		case flasher_state_t::ST_IDLE:
			chptr = strstr(buf, PREFIX);
			if (chptr != NULL) {
				remain = length - (chptr - buf) - strlen(PREFIX);

				if (remain < sizeof(fwblock)) {
					SHTRACE("fwblock fragmented!");
					memcpy(&fwblock.data[0], (chptr + strlen(PREFIX)), remain);
					remain = sizeof(fwblock) - remain;
					state = flasher_state_t::ST_HOOKED;
				} else {
					SHTRACE("fwblock fully received");
					memcpy(&fwblock.data[0], (chptr + strlen(PREFIX)), sizeof(fwblock));
					state = flasher_state_t::ST_COMPLETE;
				}

			}
			break;

		case flasher_state_t::ST_HOOKED:
			if (remain <= length) {
				memcpy(&fwblock.data[sizeof(fwblock) - remain], &buf[0], remain);
				state = flasher_state_t::ST_COMPLETE;
			} else {
				memcpy(&fwblock.data[sizeof(fwblock) - remain], &buf[0], length);
				remain = remain - length;
			}
			break;

		case flasher_state_t::ST_COMPLETE:
			validateChunk();
			flashChunk();
			cdc::WriteBlk(btldr_respond_msg[static_cast<uint8_t>(btldr_respond_t::RSPD_ACK)],
							strlen(btldr_respond_msg[static_cast<uint8_t>(btldr_respond_t::RSPD_ACK)]));

			if (fwblock.fields.block_number == (params.fields.num - 1)) {
				SHTRACE("FLASHED last chunk");
				result = true;
			} else {
				state = flasher_state_t::ST_IDLE;
				remain = 0;
			}
			break;

		default: SHTRACE("chunk parser error!"); disconnectAndReset();
		}
		return result;
	}

	static void calculateCRC() noexcept {

		for(uint32_t i = 0; i < (params.fields.num); i++) {
			flash::ReadChunk(i, &fwblock.fields.data_block[0], TBlockSize);
			crc32::CalcBuffer(&fwblock.fields.data_block[0], TBlockSize / sizeof(uint32_t));
		}


		SHTRACE("CRC: %#010x", crc32::GetResult());

		crc32::DeInit();
		crc32::Init();
		flash::ReadChunk(0, &fwblock.fields.data_block[0], TBlockSize);
		crc32::CalcBuffer(&fwblock.fields.data_block[0], 1);
		SHTRACE("CRC: %#010x", crc32::GetResult());
		SHTRACE("DT: %#010x %#010x %#010x", fwblock.fields.data_block[0], fwblock.fields.data_block[1], fwblock.fields.data_block[2]);

		led::SetAll(0);
		delay_ms(2000);
		JumpToApp(FIRMWARE_ADDR_START);
//		__BKPT(2);
//		uint32_t crc = crc32::CalcBufferBlocking(data, 3);

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
				disconnectAndReset();
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
			chptr = strstr(buf, PREFIX);
			if (t.IsTimeOut()) {
				SHTRACE("DEVNAME timeout");
				disconnectAndReset();
			}
			if (chptr != NULL) {
				chptr = strstr(buf, btldr_cmd_msg[static_cast<uint8_t>(btldr_msg_t::MSG_DEVNAME)]);
				if (chptr != NULL) {
					SHTRACE("DEVNAME detected");
					cdc::WriteBlk(btldr_respond_msg[static_cast<uint8_t>(btldr_respond_t::RSPD_ACK)],
							strlen(btldr_respond_msg[static_cast<uint8_t>(btldr_respond_t::RSPD_ACK)]));
					state = btldr_state_t::ST_PARAMS;
					t.Clear();
					t.Set(2000);
				} else {
					SHTRACE("invalid DEVNAME detected");
					cdc::WriteBlk(btldr_respond_msg[static_cast<uint8_t>(btldr_respond_t::RSPD_BAD_DEVNAME)],
									strlen(btldr_respond_msg[static_cast<uint8_t>(btldr_respond_t::RSPD_BAD_DEVNAME)]));
					disconnectAndReset();
				}
			} else {
				t.Update();
			}
			break;

		case btldr_state_t::ST_PARAMS:
			led::Set(3,1);
//			chptr = strstr(buf, PREFIX);

			if (t.IsTimeOut()) {
				SHTRACE("PARAMS timeout");
				disconnectAndReset();
			}

			if (parseParams(buf, length)) {
				SHTRACE("PARAMS parsed");
				state = btldr_state_t::ST_FLASH;
				t.Clear();
				t.Set(20000);
			} else {
				t.Update();
			}

			break;

		case btldr_state_t::ST_FLASH:
			led::Set(4,1);

			if (t.IsTimeOut()) {
				SHTRACE("FLASH timeout");
				disconnectAndReset();
			}

			if (parseChunk(buf, length)) {
				SHTRACE("FLASH finished");
				state = btldr_state_t::ST_CHECK;
				t.Clear();
				t.Set(10000);
			} else {
				t.Update();
			}

			break;

		case btldr_state_t::ST_CHECK:
			led::Set(5,1);

			if (t.IsTimeOut()) {
				SHTRACE("CHECK timeout");
				disconnectAndReset();
			}

			if (chptr != NULL) {
				chptr = strstr(buf, btldr_cmd_msg[static_cast<uint8_t>(btldr_msg_t::MSG_CHECK)]);
				if (chptr != NULL) {
					SHTRACE("CHECK detected");
					calculateCRC();
				} else {
					SHTRACE("invalid CHECK cmd detected");
					cdc::WriteBlk(btldr_respond_msg[static_cast<uint8_t>(btldr_respond_t::RSPD_BAD_FLASH_CMD)],
									strlen(btldr_respond_msg[static_cast<uint8_t>(btldr_respond_t::RSPD_BAD_FLASH_CMD)]));
					disconnectAndReset();
				}
			} else {
				t.Update();
			}

			break;

		default: disconnectAndReset();
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
//	"BTLDR_BAD_CHSZ_RATIO\n",
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
