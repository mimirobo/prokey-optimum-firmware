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

#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include <libopencm3/stm32/gpio.h>
#include <stdbool.h>

struct buttonState {
  volatile bool YesUp;
  volatile int YesDown;
  volatile bool NoUp;
  volatile int NoDown;

  //! ProKey
	volatile bool UpUp;
	volatile int UpDown;
	volatile bool DownUp;
	volatile int DownDown;
};

typedef enum _eButtons
{
	BTN_NONE= 0,
	BTN_YES = 1,
	BTN_NO  = 2,
	BTN_UP	= 4,
	BTN_DOWN= 8, 
	BTN_ALL = 0x0F,
} eButtons;

extern struct buttonState button;

uint16_t buttonRead(void);
void buttonUpdate(void);

eButtons	ButtonGet	( eButtons expected );

#ifndef BTN_PORT
#define BTN_PORT	GPIOC
#endif

#ifndef BTN_PIN_YES
#define BTN_PIN_YES		GPIO6
#endif

#ifndef BTN_PIN_NO
#define BTN_PIN_NO		GPIO9
#endif

#ifndef BTN_PIN_UP
#define BTN_PIN_UP		GPIO8
#endif

#ifndef BTN_PIN_DOWN
#define BTN_PIN_DOWN	GPIO7
#endif

#endif
