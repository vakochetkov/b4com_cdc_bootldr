/*
 * usb_cdc.hpp
 *
 *  Created on: 26 нояб. 2020 г.
 *      Author: kochetkov
 */

#ifndef __INCLUDE_USB_CDC_HPP_
#define __INCLUDE_USB_CDC_HPP_

#include <cstdint>
#include <cstring>
#include "stm32l0xx_common.hpp"
#include "rcc.hpp"
#include "ringbuffer.hpp"

extern "C" {
#include "usb.h"
#include "usb_cdc.h"
#include "retarget_bkpt.h"
}

#define CDC_EP0_SIZE    0x08
#define CDC_RXD_EP      0x01
#define CDC_TXD_EP      0x81
#define CDC_DATA_SZ     0x40
#define CDC_NTF_EP      0x82
#define CDC_NTF_SZ      0x08

#define CDC_PROTOCOL USB_PROTO_NONE // USB_CDC_PROTO_V25TER

extern "C" {

extern usbd_device usb_udev;
extern uint32_t	usb_ubuf[0x20];

extern const struct usb_string_descriptor *const dtable[];

/* Declaration of the report descriptor */
struct cdc_config {
    struct usb_config_descriptor        config;
    struct usb_iad_descriptor           comm_iad;
    struct usb_interface_descriptor     comm;
    struct usb_cdc_header_desc          cdc_hdr;
    struct usb_cdc_call_mgmt_desc       cdc_mgmt;
    struct usb_cdc_acm_desc             cdc_acm;
    struct usb_cdc_union_desc           cdc_union;
    struct usb_endpoint_descriptor      comm_ep;
    struct usb_interface_descriptor     data;
    struct usb_endpoint_descriptor      data_eprx;
    struct usb_endpoint_descriptor      data_eptx;
} __attribute__((packed));

/* Device descriptor */
static const struct usb_device_descriptor device_desc = {
    .bLength            = sizeof(struct usb_device_descriptor),
    .bDescriptorType    = USB_DTYPE_DEVICE,
    .bcdUSB             = VERSION_BCD(2,0,0),
    .bDeviceClass       = USB_CLASS_IAD,
    .bDeviceSubClass    = USB_SUBCLASS_IAD,
    .bDeviceProtocol    = USB_PROTO_IAD,
    .bMaxPacketSize0    = CDC_EP0_SIZE,
    .idVendor           = 0x16C0,
    .idProduct          = 0x05E1,
    .bcdDevice          = VERSION_BCD(1,0,0),
    .iManufacturer      = 1,
    .iProduct           = 2,
    .iSerialNumber      = INTSERIALNO_DESCRIPTOR,
    .bNumConfigurations = 1,
};

/* Device configuration descriptor */
static const struct cdc_config config_desc = {
    .config = {
        .bLength                = sizeof(struct usb_config_descriptor),
        .bDescriptorType        = USB_DTYPE_CONFIGURATION,
        .wTotalLength           = sizeof(struct cdc_config),
        .bNumInterfaces         = 2,
        .bConfigurationValue    = 1,
        .iConfiguration         = NO_DESCRIPTOR,
        .bmAttributes           = USB_CFG_ATTR_RESERVED | USB_CFG_ATTR_SELFPOWERED,
        .bMaxPower              = USB_CFG_POWER_MA(100),
    },
    .comm_iad = {
        .bLength = sizeof(struct usb_iad_descriptor),
        .bDescriptorType        = USB_DTYPE_INTERFASEASSOC,
        .bFirstInterface        = 0,
        .bInterfaceCount        = 2,
        .bFunctionClass         = USB_CLASS_CDC,
        .bFunctionSubClass      = USB_CDC_SUBCLASS_ACM,
        .bFunctionProtocol      = CDC_PROTOCOL,
        .iFunction              = NO_DESCRIPTOR,
    },
    .comm = {
        .bLength                = sizeof(struct usb_interface_descriptor),
        .bDescriptorType        = USB_DTYPE_INTERFACE,
        .bInterfaceNumber       = 0,
        .bAlternateSetting      = 0,
        .bNumEndpoints          = 1,
        .bInterfaceClass        = USB_CLASS_CDC,
        .bInterfaceSubClass     = USB_CDC_SUBCLASS_ACM,
        .bInterfaceProtocol     = CDC_PROTOCOL,
        .iInterface             = NO_DESCRIPTOR,
    },
    .cdc_hdr = {
        .bFunctionLength        = sizeof(struct usb_cdc_header_desc),
        .bDescriptorType        = USB_DTYPE_CS_INTERFACE,
        .bDescriptorSubType     = USB_DTYPE_CDC_HEADER,
        .bcdCDC                 = VERSION_BCD(1,1,0),
    },
    .cdc_mgmt = {
        .bFunctionLength        = sizeof(struct usb_cdc_call_mgmt_desc),
        .bDescriptorType        = USB_DTYPE_CS_INTERFACE,
        .bDescriptorSubType     = USB_DTYPE_CDC_CALL_MANAGEMENT,
        .bmCapabilities         = 0,
        .bDataInterface         = 1,

    },
    .cdc_acm = {
        .bFunctionLength        = sizeof(struct usb_cdc_acm_desc),
        .bDescriptorType        = USB_DTYPE_CS_INTERFACE,
        .bDescriptorSubType     = USB_DTYPE_CDC_ACM,
        .bmCapabilities         = 0,
    },
    .cdc_union = {
        .bFunctionLength        = sizeof(struct usb_cdc_union_desc),
        .bDescriptorType        = USB_DTYPE_CS_INTERFACE,
        .bDescriptorSubType     = USB_DTYPE_CDC_UNION,
        .bMasterInterface0      = 0,
        .bSlaveInterface0       = 1,
    },
    .comm_ep = {
        .bLength                = sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType        = USB_DTYPE_ENDPOINT,
        .bEndpointAddress       = CDC_NTF_EP,
        .bmAttributes           = USB_EPTYPE_INTERRUPT,
        .wMaxPacketSize         = CDC_NTF_SZ,
        .bInterval              = 0xFF,
    },
    .data = {
        .bLength                = sizeof(struct usb_interface_descriptor),
        .bDescriptorType        = USB_DTYPE_INTERFACE,
        .bInterfaceNumber       = 1,
        .bAlternateSetting      = 0,
        .bNumEndpoints          = 2,
        .bInterfaceClass        = USB_CLASS_CDC_DATA,
        .bInterfaceSubClass     = USB_SUBCLASS_NONE,
        .bInterfaceProtocol     = USB_PROTO_NONE,
        .iInterface             = NO_DESCRIPTOR,
    },
    .data_eprx = {
        .bLength                = sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType        = USB_DTYPE_ENDPOINT,
        .bEndpointAddress       = CDC_RXD_EP,
        .bmAttributes           = USB_EPTYPE_BULK,
        .wMaxPacketSize         = CDC_DATA_SZ,
        .bInterval              = 0x01,
    },
    .data_eptx = {
        .bLength                = sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType        = USB_DTYPE_ENDPOINT,
        .bEndpointAddress       = CDC_TXD_EP,
        .bmAttributes           = USB_EPTYPE_BULK,
        .wMaxPacketSize         = CDC_DATA_SZ,
        .bInterval              = 0x01,
    },
};

static struct usb_cdc_line_coding cdc_line = {
    .dwDTERate          = 250000,
    .bCharFormat        = USB_CDC_1_STOP_BITS,
    .bParityType        = USB_CDC_NO_PARITY,
    .bDataBits          = 8,
};
} // extern "C"

constexpr uint32_t USB_BUF_SIZE = 512; // 8 * CDC_DATA_SZ

extern jnk0le::Ringbuffer<char, USB_BUF_SIZE> rxbuf;
extern jnk0le::Ringbuffer<char, USB_BUF_SIZE> txbuf;

class usb_cdc_c {

	static inline void flushTxBuffer() {
		txbuf.consumerClear();
	}

	static inline void flushRxBuffer() {
		rxbuf.producerClear();
	}

	static usbd_respond SetConfig(usbd_device *dev, uint8_t cfg) noexcept {
		switch (cfg) {
		case 0:
			/* deconfiguring device */
			usbd_ep_deconfig(dev, CDC_NTF_EP);
			usbd_ep_deconfig(dev, CDC_TXD_EP);
			usbd_ep_deconfig(dev, CDC_RXD_EP);
			usbd_reg_endpoint(dev, CDC_RXD_EP, 0);
			usbd_reg_endpoint(dev, CDC_TXD_EP, 0);
			return usbd_ack;
		case 1:
			/* configuring device */
			usbd_ep_config(dev, CDC_RXD_EP, USB_EPTYPE_BULK /*| USB_EPTYPE_DBLBUF*/, CDC_DATA_SZ);
			usbd_ep_config(dev, CDC_TXD_EP, USB_EPTYPE_BULK /*| USB_EPTYPE_DBLBUF*/, CDC_DATA_SZ);
			usbd_ep_config(dev, CDC_NTF_EP, USB_EPTYPE_INTERRUPT, CDC_NTF_SZ);

#if ((CDC_TXD_EP & 0x7F) == (CDC_RXD_EP & 0x7F))
			usbd_reg_endpoint(dev, CDC_RXD_EP, usb_cdc_c::HandleData);
			usbd_reg_endpoint(dev, CDC_TXD_EP, usb_cdc_c::HandleData);
#else
#error "Bi-directional endpoint handlers not implemented!"
#endif

			usbd_ep_write(dev, CDC_TXD_EP, 0, 0);
			return usbd_ack;
		default:
			return usbd_fail;
		}
	}

	static usbd_respond GetDescriptor(usbd_ctlreq *req, void **address, uint16_t *length) noexcept {
		const uint8_t dtype = req->wValue >> 8;
		const uint8_t dnumber = req->wValue & 0xFF;
		const void* desc;
		uint16_t len = 0;
		switch (dtype) {
		case USB_DTYPE_DEVICE:
			desc = &device_desc;
			break;
		case USB_DTYPE_CONFIGURATION:
			desc = &config_desc;
			len = sizeof(config_desc);
			break;
		case USB_DTYPE_STRING:
			if (dnumber < 3) {
				desc = dtable[dnumber];
			} else {
				return usbd_fail;
			}
			break;
		default:
			return usbd_fail;
		}
		if (len == 0) {
			len = ((struct usb_header_descriptor*)desc)->bLength;
		}
		*address = (void*)desc;
		*length = len;
		return usbd_ack;
	}

	static usbd_respond ContorlHandler(usbd_device *dev, usbd_ctlreq *req, usbd_rqc_callback *callback) noexcept {
		if (((USB_REQ_RECIPIENT | USB_REQ_TYPE) & req->bmRequestType) == (USB_REQ_INTERFACE | USB_REQ_CLASS)
				&& req->wIndex == 0)
		{
			switch (req->bRequest) {
			case USB_CDC_SET_CONTROL_LINE_STATE:
				return usbd_ack;
			case USB_CDC_SET_LINE_CODING:
				memcpy( req->data, &cdc_line, sizeof(cdc_line));
				return usbd_ack;
			case USB_CDC_GET_LINE_CODING:
				dev->status.data_ptr = &cdc_line;
				dev->status.data_count = sizeof(cdc_line);
				return usbd_ack;
			default:
				return usbd_fail;
			}
		}
		return usbd_fail;
	}

	static void inline HandleData(usbd_device *dev, uint8_t event, uint8_t ep) noexcept {
		if (event == usbd_evt_eptx) {
			HandleWrite(dev, event, ep);
		} else {
			HandleRead(dev, event, ep);
		}
	}

	/**
	 * Write new data to IN EP (if txbuf not empty)
	 * @param dev   - UDEV structure
	 * @param event - polled USB event
	 * @param ep    - triggered TX EP
	 */
	static void HandleWrite(usbd_device *dev, uint8_t event, uint8_t ep) noexcept {
		static char tmpbuf[CDC_DATA_SZ] = {0};
		auto freeLen = txbuf.readAvailable();

		if (freeLen) {
			auto blen =  (freeLen < CDC_DATA_SZ) ? freeLen : CDC_DATA_SZ;
			txbuf.readBuff(&tmpbuf[0], blen);
			usbd_ep_write(dev, ep, &tmpbuf[0], blen);
		}
	}

	/**
	 * Read new data from OUT EP (if there is enough space in rxbuf)
	 * @param dev   - UDEV structure
	 * @param event - polled USB event
	 * @param ep    - triggered RX EP
	 */
	static void HandleRead (usbd_device *dev, uint8_t event, uint8_t ep) noexcept {
		static char tmpbuf[CDC_DATA_SZ] = {0};
		int32_t rlen;
		auto freeLen = rxbuf.writeAvailable();

		if (freeLen) {
			rlen = usbd_ep_read(dev, ep, &tmpbuf[0], (freeLen < CDC_DATA_SZ) ? freeLen : CDC_DATA_SZ);
			if (rlen > 0) {
				rxbuf.writeBuff(&tmpbuf[0], rlen);
			}
		}
	}

	static inline void ENABLE_IRQ() noexcept {
		NVIC_EnableIRQ(USB_IRQn);
	}

	static inline void DISABLE_IRQ() noexcept {
		NVIC_DisableIRQ(USB_IRQn);
	}

public:
	static void Init() noexcept {

		usbd_init(&usb_udev, &usbd_hw, CDC_EP0_SIZE, usb_ubuf, sizeof(usb_ubuf));
		usbd_reg_config(&usb_udev,  usb_cdc_c::SetConfig);
		usbd_reg_control(&usb_udev, usb_cdc_c::ContorlHandler);
		usbd_reg_descr(&usb_udev,   usb_cdc_c::GetDescriptor);

		NVIC_SetPriority(USB_IRQn, 3); // 0 is the highest (L0xx has no subprio)
		ENABLE_IRQ();
		usbd_enable(&usb_udev, true);
		usbd_connect(&usb_udev, true);
	}

	static void DeInit() noexcept {
		usbd_connect(&usb_udev, false);
		usbd_enable(&usb_udev, false);
	}

	static void WriteCharNonBlk(char ch) noexcept {
		txbuf.insert(ch);
	}

	static void WriteNonBlk(const char * msg, uint32_t length) noexcept {
		if (msg == nullptr) {
			return;
		}
		if (!length) {
			return;
		}
		if (length > txbuf.writeAvailable()) {
			return;
		}

		txbuf.writeBuff(msg, length);
	}

	static inline void WriteCharBlk(char ch) {
		WriteCharNonBlk(ch);
		FlushTx();
	}

	static void inline WriteBlk(const char * msg, uint32_t length) noexcept {
		WriteNonBlk(msg, length);
		FlushTx();
	}

	/**
	 * Push TX FIFO to TX endpoint in chunks of CDC_DATA_SZ or less
	 */
	static void FlushTx() {
		static char tmpbuf[CDC_DATA_SZ] = {0};
		auto freeLen = txbuf.readAvailable();

		while (freeLen) {
			txbuf.readBuff(&tmpbuf[0], (freeLen < CDC_DATA_SZ) ? freeLen : CDC_DATA_SZ);
			usbd_ep_write(&usb_udev, CDC_TXD_EP, &tmpbuf[0], (freeLen < CDC_DATA_SZ) ? freeLen : CDC_DATA_SZ);
			freeLen = txbuf.readAvailable();
			delay_ms(5); // litlle delay to avoid IRQ overflood
		}
	}

	static void Read(char * msg, uint32_t length) noexcept {
		if (msg == nullptr) {
			return;
		}
		if (!length) {
			return;
		}
		if (length > USB_BUF_SIZE) {
			return;
		}

		auto available = rxbuf.readAvailable();
		rxbuf.readBuff(&msg[0], (available < length) ? available : length);
	}

	static char ReadChar() noexcept { //FIXME: что-то тут не так
		char tmp;
		if (rxbuf.readAvailable() == 0) {
			return 0;
		}

		rxbuf.remove(&tmp);
		return tmp;
	}
};

typedef usb_cdc_c cdc;

#endif /* __INCLUDE_USB_CDC_HPP_ */
