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

	cdc_init_usbd();
	usbd_enable(&usb_udev, true);
	usbd_connect(&usb_udev, true);
	while(1) {
		usbd_poll(&usb_udev);
	}
}



