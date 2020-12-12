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
		while(!t.IsAlarmed()) {
			t.Check();
		}
	}


};

template<size_t TBlockSize>
const char * bootloader_c<TBlockSize>::btldr_cmd_msg[] = {
    "BTLDR_START",
};


typedef bootloader_c<256> bootloader;


#endif /* INCLUDE_BOOTLOADER_HPP_ */
