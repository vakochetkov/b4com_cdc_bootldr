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

	bootloader::Init();
	bootloader::SelfTest();

	cdc::Init(); // TODO: after succesfull check ONLY!


	while(1) {
//		auto rxlen = cdc::GetRxLen();
//		auto txlen = cdc::GetTxLen();
		cdc::Read(buffer, BUFSIZE);
		bootloader::ProcessNext(buffer, BUFSIZE);

////		cdc::WriteCharNonBlk('0' + i);
////		cdc::FlushTx();
		cdc::ReadChar();
//		sprintf(chs, "\nr:%d t:%d\n", rxlen, txlen);
//		cdc::WriteBlk(chs, 20);
		delay_ms(10);
//
////		SHTRACE("ch %c", ch);
//		SHTRACE("TX %lu RX %lu", txlen, rxlen);
////		delay_ms(500);

	}
}



