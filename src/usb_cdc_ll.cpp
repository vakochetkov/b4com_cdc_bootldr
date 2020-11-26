/*
 * usb_cdc.cpp
 * Implementation of USB CDC class based in libusb_stm32 demo
 *
 *  Created on: 26 нояб. 2020 г.
 *      Author: kochetkov
 */

#include "usb_cdc_ll.hpp"

// UDEV
usbd_device usb_udev;
uint32_t	usb_ubuf[0x20];

extern "C" {
void USB_IRQHandler(void)
{
	usbd_poll(&usb_udev);
}
}

uint8_t  usb_cdc_c::fifo[USB_FIFO_SIZE];
uint32_t usb_cdc_c::fpos;


