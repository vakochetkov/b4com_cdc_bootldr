#include <cstdint>
#include "stm32l0xx_common.hpp"
#include "rcc.hpp"
#include "gpio.hpp"
#include "led.hpp"
#include "cdc.hpp"
#include "bootloader.hpp"

extern "C" {
#include "retarget_bkpt.h"
}

static constexpr uint32_t BUFSIZE = 64;
static char buffer[BUFSIZE];

int main() {

	rcc::InitClock();
	rcc::InitSysTick();
	gpio::Init();

	led::Init();
	led::SetPython();

	bootloader::Init(); // if don't need update, jump to user firmware
//	bootloader::SelfTest();

	cdc::Init();


	while(1) {

//		while(1) { // tx test
//			cdc::WriteBlk("BTLDR_ACK\n",10);
//			delay_ms(10);
//		}

//		char ch = '@'; // rx echo test
//		while(1) {
//			ch = cdc::ReadChar();
//			if (ch != 0)
//				cdc::WriteCharBlk(ch);
//		}
		memset(buffer, 0, BUFSIZE); // del old message if any
		cdc::Read(buffer, BUFSIZE);
		bootloader::ProcessNext(buffer, BUFSIZE);

//		SHTRACE("%s", buffer);
//		delay_ms(100);
	}
}



