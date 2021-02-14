/*
 * usb_desc.c
 *
 *  Created on: 30 нояб. 2020 г.
 *      Author: kochetkov
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stm32.h"
#include "usb.h"
#include "usb_cdc.h"

static const struct usb_string_descriptor lang_desc     = USB_ARRAY_DESC(USB_LANGID_ENG_US);
static const struct usb_string_descriptor manuf_desc_en = USB_STRING_DESC("STM32 CDC");
static const struct usb_string_descriptor prod_desc_en  = USB_STRING_DESC("CDC BOOTLOADER V1");

const struct usb_string_descriptor *const dtable[] = {
    &lang_desc,
    &manuf_desc_en,
    &prod_desc_en,
};

