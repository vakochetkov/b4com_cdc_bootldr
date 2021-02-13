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

static constexpr uint32_t BUFSIZE = 32;
static char buffer[BUFSIZE];

int main() {

	rcc::InitClock();
	rcc::InitSysTick();
	gpio::Init();

	led::Init();
	led::SetPython();

	bootloader::Init(); // if don't need update, jump to user firmware
//	bootloader::SelfTest();

	cdc::Init(); // TODO: after succesfull check ONLY!


	while(1) {
		volatile auto rxlen = cdc::GetRxLen();
//		auto txlen = cdc::GetTxLen();

//		while(1) {
//			cdc::WriteBlk("BTLDR_ACK\n",10);
//			delay_ms(500);
//		}

		cdc::Read(buffer, BUFSIZE);
		bootloader::ProcessNext(buffer, BUFSIZE);
	}
}



