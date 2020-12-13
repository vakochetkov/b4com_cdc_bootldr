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
#include "flash.hpp"
#include "timeout.hpp"

enum class btldr_magic_word_t : uint32_t {
	MW_FLASH_EMPTY       = 0xFFFFFFFF,
	MW_FLASH_ZERO        = 0x00000000,
	MW_FLASH_IS_UPDATED  = 0xB4C0CCCC,
	MW_FLASH_NEED_UPDATE = 0xB4C0AAAA
};

template<size_t TBlockSize> // FW block size
class bootloader_c {
	enum class btldr_cmd_t : uint8_t {
		CMD_START = 0,
	};
	static const char * btldr_cmd_msg[];

public:
	static void Init() noexcept {
		flash::Init();

		Timeout t;
		t.Set(100);
		while(!t.IsTimeOut()) {
			t.Update();
		}
		t.Clear();
	}


};

template<size_t TBlockSize>
const char * bootloader_c<TBlockSize>::btldr_cmd_msg[] = {
    "BTLDR_START",
};


typedef bootloader_c<256> bootloader;


#endif /* INCLUDE_BOOTLOADER_HPP_ */
