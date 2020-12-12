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

char ch = '@';

int main() {

	rcc::InitClock();
	rcc::InitSysTick();
	gpio::Init();

	led::Init();
	cdc::Init();
	bootloader::Init();

	while(1) {
		auto rxlen = cdc::GetRxLen();
		auto txlen = cdc::GetTxLen();

////		cdc::WriteCharNonBlk('0' + i);
////		cdc::FlushTx();
		ch = cdc::ReadChar();
		if (ch != 0) {
			cdc::WriteCharBlk(ch);
			delay_ms(10);
		}
//
////		SHTRACE("ch %c", ch);
//		SHTRACE("TX %lu RX %lu", txlen, rxlen);
////		delay_ms(500);

	}
}



