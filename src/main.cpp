#include <cstdint>
#include "stm32l0xx_common.hpp"
#include "rcc.hpp"
#include "gpio.hpp"
#include "led.hpp"
#include "usb_cdc_ll.hpp"

extern usbd_device usb_udev;

int main() {

	rcc::InitClock();
	rcc::InitSysTick();
	gpio::Init();

	led::Init();
	usb::Init();

	while(1) {

	}
}



