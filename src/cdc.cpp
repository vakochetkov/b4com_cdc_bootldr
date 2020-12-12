/*
 * cdc.cpp
 *
 *  Created on: 30 нояб. 2020 г.
 *      Author: kochetkov
 */

#include "cdc.hpp"

uint8_t usb_cdc_c::txbuf[USB_BUF_SIZE];
int32_t volatile usb_cdc_c::txpos;
uint8_t usb_cdc_c::rxbuf[USB_BUF_SIZE];
int32_t volatile usb_cdc_c::rxpos;

extern "C" {

usbd_device usb_udev;
uint32_t	usb_ubuf[0x20];

void USB_IRQHandler(void) {
	usbd_poll(&usb_udev);
}
}

