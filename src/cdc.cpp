/*
 * cdc.cpp
 *
 *  Created on: 30 нояб. 2020 г.
 *      Author: kochetkov
 */

#include "cdc.hpp"


extern "C" {

usbd_device usb_udev;
uint32_t	usb_ubuf[0x20];

void USB_IRQHandler(void) {
	usbd_poll(&usb_udev);
}
}

jnk0le::Ringbuffer<char, USB_BUF_SIZE> rxbuf;
jnk0le::Ringbuffer<char, USB_BUF_SIZE> txbuf;

