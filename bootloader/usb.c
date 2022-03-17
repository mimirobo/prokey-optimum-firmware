/*
 * This file is part of the TREZOR project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/flash.h>
#include <libopencm3/usb/usbd.h>

#include <string.h>

#include "bootloader.h"
#include "buttons.h"
#include "ecdsa.h"
#include "layout.h"
#include "memory.h"
#include "memzero.h"
#include "oled.h"
#include "rng.h"
#include "secp256k1.h"
#include "sha2.h"
#include "aes/aes.h"
#include "signatures.h"
#include "usb.h"
#include "util.h"
#include "auth.h"

#include <libopencm3/usb/dwc/otg_fs.h>
#include <libopencm3/usb/dwc/otg_common.h>

#include "usb21_standard.h"
#include "webusb.h"
#include "winusb.h"

#include "usb_desc.h"
#include "usb_erase.h"
#include "commands.h"


static uint32_t buttonsTestCounter = 0;
static uint32_t buttonsTestThreshold = 4000000;



static void rx_callback(usbd_device *dev, uint8_t ep) {
  (void)ep;
  static uint8_t buf[64] __attribute__((aligned(4)));

  if (usbd_ep_read_packet(dev, ENDPOINT_ADDRESS_OUT, buf, 64) != 64) return;

  CmdOnReceive(dev, buf);
}

static void set_config(usbd_device *dev, uint16_t wValue) {
  (void)wValue;

  usbd_ep_setup(dev, ENDPOINT_ADDRESS_IN, USB_ENDPOINT_ATTR_INTERRUPT, 64, 0);
  usbd_ep_setup(dev, ENDPOINT_ADDRESS_OUT, USB_ENDPOINT_ATTR_INTERRUPT, 64,
                rx_callback);
}

static usbd_device *usbd_dev;
static uint8_t usbd_control_buffer[256] __attribute__((aligned(2)));

static const struct usb_device_capability_descriptor *capabilities[] = {
    (const struct usb_device_capability_descriptor
         *)&webusb_platform_capability_descriptor,
};

static const struct usb_bos_descriptor bos_descriptor = {
    .bLength = USB_DT_BOS_SIZE,
    .bDescriptorType = USB_DT_BOS,
    .bNumDeviceCaps = sizeof(capabilities) / sizeof(capabilities[0]),
    .capabilities = capabilities};

static void usbInit(void) {
  usbd_dev = usbd_init(&otgfs_usb_driver, &dev_descr, &config, usb_strings,
                       sizeof(usb_strings) / sizeof(const char *),
                       usbd_control_buffer, sizeof(usbd_control_buffer));

  //! Force USB to work without VBUS_DET
  OTG_FS_GCCFG = 0;
	OTG_FS_GCCFG = OTG_GCCFG_NOVBUSSENS | OTG_GCCFG_PWRDWN;

  usbd_register_set_config_callback(usbd_dev, set_config);
  usb21_setup(usbd_dev, &bos_descriptor);
  webusb_setup(usbd_dev, "prokey.io");
  winusb_setup(usbd_dev, USB_INTERFACE_INDEX_MAIN);
}

static void testButtons(void)
{
  uint16_t key = gpio_port_read(BTN_PORT);

  if((key & (BTN_PIN_YES | BTN_PIN_NO | BTN_PIN_DOWN | BTN_PIN_UP)) != (BTN_PIN_YES | BTN_PIN_NO | BTN_PIN_DOWN | BTN_PIN_UP))
  {
    if(buttonsTestCounter++ >= buttonsTestThreshold)
    {
      buttonsTestCounter = 0;
      buttonsTestThreshold = 10000;

      oledClear();
      oledDrawString(0,0, "TEST BUTTONS", FONT_STANDARD);

      key = gpio_port_read(BTN_PORT);

      int x = 0;
      if((key & BTN_PIN_NO) != BTN_PIN_NO)
        x = 3;
      else if((key & BTN_PIN_UP) != BTN_PIN_UP)
        x = 50;
      else if((key & BTN_PIN_DOWN) != BTN_PIN_DOWN)
        x = 80;
      else if((key & BTN_PIN_YES) != BTN_PIN_YES)
        x = OLED_WIDTH - 10;

      oledDrawString(x, OLED_HEIGHT - 8, "*", FONT_STANDARD);
      oledRefresh();
    }
  }
  else
  {
    buttonsTestCounter = 0;
  }
}


void usbLoop(void) {
  usbInit();
  for (;;) {
    usbd_poll(usbd_dev);
    
    testButtons();
  }
}
