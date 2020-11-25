/*
 * usb_cdc_str_desc.c
 *
 *  Created on: 26 нояб. 2020 г.
 *      Author: kochetkov
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "usb.h"

static const struct usb_string_descriptor lang_desc     = USB_ARRAY_DESC(USB_LANGID_ENG_US);
static const struct usb_string_descriptor manuf_desc_en = USB_STRING_DESC("Open source USB stack for STM32");
static const struct usb_string_descriptor prod_desc_en  = USB_STRING_DESC("CDC Loopback demo");
const struct usb_string_descriptor *const string_desc_table[] = {
    &lang_desc,
    &manuf_desc_en,
    &prod_desc_en,
};

#ifdef __cplusplus
}
#endif
