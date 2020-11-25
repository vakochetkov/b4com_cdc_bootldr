#define RELAY_COUNT 12

#include <cstdint>
#include "stm32l0xx_common.hpp"
#include "rcc.hpp"
#include "gpio.hpp"
#include "led.hpp"
#include "relay.hpp"
#include "input_ctrl.hpp"

int main() {

	rcc::InitClock();
	rcc::InitSysTick();
	gpio::Init();

	led::Init();
	relay::Init();
//	ictrl::Init();

	while(1) {



	}
}



