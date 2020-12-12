/*
 * bootloader.hpp
 *
 *  Created on: 13 дек. 2020 г.
 *      Author: kochetkov
 */

#ifndef INCLUDE_BOOTLOADER_HPP_
#define INCLUDE_BOOTLOADER_HPP_

#include "rcc.hpp"
#include "led.hpp"
#include "cdc.hpp"

template<size_t TBlockSize> // FW block size
class bootloader_c {
	enum class btldr_cmd_t : uint8_t {
		BL_CMD_START = 0,
	};
	static const char * btldr_cmd_msg[];

public:
	static void Init() noexcept {

	}


};

template<size_t TBlockSize>
const char * bootloader_c<TBlockSize>::btldr_cmd_msg[] = {
    "BTLDR_START",
};


typedef bootloader_c<256> bootloader;


#endif /* INCLUDE_BOOTLOADER_HPP_ */
